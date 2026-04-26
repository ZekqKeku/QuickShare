#include <gtest/gtest.h>
#include <QSignalSpy>
#include <QCoreApplication>
#include "../src/network/PixeldrainUploader.h"

class NetworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!QCoreApplication::instance()) {
            int argc = 1;
            char *argv[] = {(char*)"test"};
            app = new QCoreApplication(argc, argv);
        }
    }

    void TearDown() override {
    }

    QCoreApplication* app = nullptr;
};

TEST_F(NetworkTest, UploadNonExistentFileEmitsError) {
    PixeldrainUploader uploader;
    QSignalSpy spy(&uploader, &PixeldrainUploader::uploadError);

    uploader.uploadFile("/path/to/non/existent/file/that/really/should/not/exist/12345");

    EXPECT_EQ(spy.count(), 1);
    QString errorMsg = spy.at(0).at(0).toString();
    EXPECT_TRUE(errorMsg.contains("Cannot open file"));
}
