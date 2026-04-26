#include <gtest/gtest.h>
#include "../src/utils/NameGenerator.h"
#include "../src/utils/Archiver.h"
#include <QFile>
#include <QTemporaryDir>
#include <QTemporaryFile>

TEST(NameGeneratorTest, GeneratesCorrectLength) {
    int length = 12;
    QString name = NameGenerator::generateRandomName(length);
    EXPECT_EQ(name.length(), length);
}

TEST(NameGeneratorTest, GeneratesDifferentNames) {
    QString name1 = NameGenerator::generateRandomName(8);
    QString name2 = NameGenerator::generateRandomName(8);
    EXPECT_NE(name1, name2);
}

TEST(ArchiverTest, CreateZipWithDummyFiles) {
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    QString file1Path = dir.filePath("test1.txt");
    QFile file1(file1Path);
    ASSERT_TRUE(file1.open(QIODevice::WriteOnly));
    file1.write("test content 1");
    file1.close();

    QString zipPath = dir.filePath("output.zip");
    QStringList filesToZip;
    filesToZip << file1Path;

    QString result = Archiver::createZip(filesToZip, zipPath);
    
    if (!result.isEmpty()) {
        EXPECT_TRUE(QFile::exists(zipPath));
    }
}
