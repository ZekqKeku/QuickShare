#include <gtest/gtest.h>
#include "../src/cli/CliHandler.h"
#include "../src/core/SettingsManager.h"
#include <QStringList>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <iostream>
#include <sstream>

class CliHandlerTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        int argc = 1;
        static char* argv[] = {(char*)"QuickShareTests"};
        static QCoreApplication app(argc, argv);
        QCoreApplication::setOrganizationName("QuickShareTests");
        QCoreApplication::setApplicationName("QuickShareCliTest");
    }

    void SetUp() override {
        SettingsManager settings;
        settings.removeAllData();
    }

    void TearDown() override {
        SettingsManager settings;
        settings.removeAllData();
    }
};

TEST_F(CliHandlerTest, ConfigApiSetsKey) {
    CliHandler handler;
    QStringList args = {"qs", "config", "api", "test-api-key"};
    
    int result = handler.execute(args);
    EXPECT_EQ(result, 0);

    SettingsManager settings;
    EXPECT_EQ(settings.apiKey(), "test-api-key");
}

TEST_F(CliHandlerTest, ConfigAutostartSetsValue) {
    CliHandler handler;
    
    handler.execute({"qs", "config", "autostart", "true"});
    EXPECT_TRUE(SettingsManager().startWithSystem());

    handler.execute({"qs", "config", "autostart", "false"});
    EXPECT_FALSE(SettingsManager().startWithSystem());
}

TEST_F(CliHandlerTest, ConfigHideInTaskbarSetsValue) {
    CliHandler handler;
    
    handler.execute({"qs", "config", "hide-in-taskbar", "true"});
    EXPECT_TRUE(SettingsManager().minimizeToTray());

    handler.execute({"qs", "config", "hide-in-taskbar", "false"});
    EXPECT_FALSE(SettingsManager().minimizeToTray());
}

TEST_F(CliHandlerTest, ConfigUnknownOptionReturnsError) {
    CliHandler handler;
    QStringList args = {"qs", "config", "unknown", "value"};
    
    int result = handler.execute(args);
    EXPECT_NE(result, 0);
}

TEST_F(CliHandlerTest, UploadFailsWithoutApiKeyAndShowsLink) {
    CliHandler handler;
    QStringList args = {"qs", "somefile.txt"};
    
    SettingsManager settings;
    settings.setApiKey("");
    settings.save();

    std::stringstream ss;
    std::streambuf* orig_cerr = std::cerr.rdbuf(ss.rdbuf());

    int result = handler.execute(args);
    std::cerr.rdbuf(orig_cerr);

    EXPECT_EQ(result, 1);
    EXPECT_TRUE(ss.str().find("https://pixeldrain.com/user/api_keys") != std::string::npos);
}

TEST_F(CliHandlerTest, SoloUploadCreatesReportFile) {
    CliHandler handler;
    SettingsManager settings;
    settings.setApiKey("dummy-key");
    settings.save();

    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QString filePath = dir.filePath("testfile.txt");
    QFile f(filePath);
    ASSERT_TRUE(f.open(QIODevice::WriteOnly));
    f.write("test content");
    f.close();

    QStringList args = {"qs", "-s", filePath};
}

TEST_F(CliHandlerTest, CompressAndSoloConflict) {
    CliHandler handler;
    SettingsManager settings;
    settings.setApiKey("dummy-key");
    settings.save();

    QStringList args = {"qs", "-c", "-s", "nonexistent.txt"};
    int result = handler.execute(args);
    EXPECT_EQ(result, 1);
}

TEST_F(CliHandlerTest, VersionOption) {
    CliHandler handler;
    QStringList args = {"qs", "--version"};
    int result = handler.execute(args);
    EXPECT_EQ(result, 0);
}

TEST_F(CliHandlerTest, UseTemporaryApiKey) {
    CliHandler handler;
    SettingsManager settings;
    settings.setApiKey("permanent-key");
    settings.save();

    QStringList args = {"qs", "-a", "temp-key", "nonexistent.txt"};
    int result = handler.execute(args);
    EXPECT_EQ(result, 1);
}

TEST_F(CliHandlerTest, HelpOption) {
    CliHandler handler;
    QStringList args = {"qs", "--help"};
    int result = handler.execute(args);
    EXPECT_EQ(result, 0);
}

TEST_F(CliHandlerTest, DirectoryHandlingRequiresInteraction) {
    CliHandler handler;
    SettingsManager settings;
    settings.setApiKey("dummy-key");
    settings.save();

    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    std::stringstream ss;
    ss << "n\nn\n";
    std::streambuf* orig_cin = std::cin.rdbuf(ss.rdbuf());

    QStringList args = {"qs", dir.path()};
    int result = handler.execute(args);
    
    std::cin.rdbuf(orig_cin);
    EXPECT_EQ(result, 0);
}
