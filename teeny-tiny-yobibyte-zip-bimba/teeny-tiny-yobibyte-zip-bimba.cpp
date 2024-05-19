#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdint>
#include <cmath>
#include <zlib.h>

// not working yet :(


void writeLocalFileHeader(std::ostream& out, const std::string& filename, uint32_t compressedSize, uint64_t uncompressedSize, uint32_t crc32) {
    uint32_t signature = 0x04034b50;
    uint16_t version = 45;
    uint16_t flags = 0;
    uint16_t compression = 8;
    uint16_t modTime = 0;
    uint16_t modDate = 0;
    uint16_t filenameLength = filename.length();
    uint16_t extraFieldLength = 20;

    out.write(reinterpret_cast<char*>(&signature), sizeof(signature));
    out.write(reinterpret_cast<char*>(&version), sizeof(version));
    out.write(reinterpret_cast<char*>(&flags), sizeof(flags));
    out.write(reinterpret_cast<char*>(&compression), sizeof(compression));
    out.write(reinterpret_cast<char*>(&modTime), sizeof(modTime));
    out.write(reinterpret_cast<char*>(&modDate), sizeof(modDate));
    out.write(reinterpret_cast<char*>(&crc32), sizeof(crc32));
    out.write(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
    uint32_t maxUint32 = 0xFFFFFFFF;
    out.write(reinterpret_cast<char*>(&maxUint32), sizeof(maxUint32));
    out.write(reinterpret_cast<char*>(&maxUint32), sizeof(maxUint32));
    out.write(reinterpret_cast<char*>(&filenameLength), sizeof(filenameLength));
    out.write(reinterpret_cast<char*>(&extraFieldLength), sizeof(extraFieldLength));
    out.write(filename.c_str(), filenameLength);

    uint16_t zip64HeaderId = 0x0001;
    uint16_t zip64DataSize = 16;
    out.write(reinterpret_cast<char*>(&zip64HeaderId), sizeof(zip64HeaderId));
    out.write(reinterpret_cast<char*>(&zip64DataSize), sizeof(zip64DataSize));
    out.write(reinterpret_cast<char*>(&uncompressedSize), sizeof(uncompressedSize));
    out.write(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
}

void writeCentralDirectoryHeader(std::ostream& out, const std::string& filename, uint32_t compressedSize, uint64_t uncompressedSize, uint32_t crc32, uint32_t localHeaderOffset) {
    uint32_t signature = 0x02014b50;
    uint16_t versionMadeBy = 45;
    uint16_t versionNeeded = 45;
    uint16_t flags = 0;
    uint16_t compression = 8; // DEFLATE
    uint16_t modTime = 0;
    uint16_t modDate = 0;
    uint16_t filenameLength = filename.length();
    uint16_t extraFieldLength = 20;
    uint16_t fileCommentLength = 0;
    uint16_t diskNumberStart = 0;
    uint16_t internalFileAttributes = 0;
    uint32_t externalFileAttributes = 0;
    uint32_t relativeOffset = localHeaderOffset;

    out.write(reinterpret_cast<char*>(&signature), sizeof(signature));
    out.write(reinterpret_cast<char*>(&versionMadeBy), sizeof(versionMadeBy));
    out.write(reinterpret_cast<char*>(&versionNeeded), sizeof(versionNeeded));
    out.write(reinterpret_cast<char*>(&flags), sizeof(flags));
    out.write(reinterpret_cast<char*>(&compression), sizeof(compression));
    out.write(reinterpret_cast<char*>(&modTime), sizeof(modTime));
    out.write(reinterpret_cast<char*>(&modDate), sizeof(modDate));
    out.write(reinterpret_cast<char*>(&crc32), sizeof(crc32));
    uint32_t maxUint32 = 0xFFFFFFFF;
    out.write(reinterpret_cast<char*>(&maxUint32), sizeof(maxUint32));
    out.write(reinterpret_cast<char*>(&maxUint32), sizeof(maxUint32));
    out.write(reinterpret_cast<char*>(&filenameLength), sizeof(filenameLength));
    out.write(reinterpret_cast<char*>(&extraFieldLength), sizeof(extraFieldLength));
    out.write(reinterpret_cast<char*>(&fileCommentLength), sizeof(fileCommentLength));
    out.write(reinterpret_cast<char*>(&diskNumberStart), sizeof(diskNumberStart));
    out.write(reinterpret_cast<char*>(&internalFileAttributes), sizeof(internalFileAttributes));
    out.write(reinterpret_cast<char*>(&externalFileAttributes), sizeof(externalFileAttributes));
    out.write(reinterpret_cast<char*>(&relativeOffset), sizeof(relativeOffset));
    out.write(filename.c_str(), filenameLength);

    uint16_t zip64HeaderId = 0x0001;
    uint16_t zip64DataSize = 16;
    out.write(reinterpret_cast<char*>(&zip64HeaderId), sizeof(zip64HeaderId));
    out.write(reinterpret_cast<char*>(&zip64DataSize), sizeof(zip64DataSize));
    out.write(reinterpret_cast<char*>(&uncompressedSize), sizeof(uncompressedSize));
    out.write(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));
}

void writeEndOfCentralDirectoryRecord(std::ostream& out, uint64_t centralDirectorySize, uint64_t centralDirectoryOffset) {
    uint32_t signature = 0x06054b50;
    uint16_t diskNumber = 0;
    uint16_t centralDirectoryDiskNumber = 0;
    uint16_t entriesThisDisk = 0xFFFF;
    uint16_t totalEntriesCentralDir = 0xFFFF;
    uint32_t sizeOfCentralDirectory = 0xFFFFFFFF;
    uint32_t offsetOfCentralDirectory = 0xFFFFFFFF;
    uint16_t commentLength = 0;

    out.write(reinterpret_cast<char*>(&signature), sizeof(signature));
    out.write(reinterpret_cast<char*>(&diskNumber), sizeof(diskNumber));
    out.write(reinterpret_cast<char*>(&centralDirectoryDiskNumber), sizeof(centralDirectoryDiskNumber));
    out.write(reinterpret_cast<char*>(&entriesThisDisk), sizeof(entriesThisDisk));
    out.write(reinterpret_cast<char*>(&totalEntriesCentralDir), sizeof(totalEntriesCentralDir));
    out.write(reinterpret_cast<char*>(&sizeOfCentralDirectory), sizeof(sizeOfCentralDirectory));
    out.write(reinterpret_cast<char*>(&offsetOfCentralDirectory), sizeof(offsetOfCentralDirectory));
    out.write(reinterpret_cast<char*>(&commentLength), sizeof(commentLength));

    uint32_t zip64EocdSignature = 0x06064b50;
    uint64_t zip64EocdSize = 44;
    uint16_t versionMadeBy = 45;
    uint16_t versionNeeded = 45;
    uint32_t diskNumberZip64 = 0;
    uint32_t centralDirectoryDiskNumberZip64 = 0;
    uint64_t totalEntriesZip64 = 1;
    uint64_t totalEntriesCentralDirZip64 = 1;
    uint64_t sizeOfCentralDirectoryZip64 = centralDirectorySize;
    uint64_t offsetOfCentralDirectoryZip64 = centralDirectoryOffset;

    out.write(reinterpret_cast<char*>(&zip64EocdSignature), sizeof(zip64EocdSignature));
    out.write(reinterpret_cast<char*>(&zip64EocdSize), sizeof(zip64EocdSize));
    out.write(reinterpret_cast<char*>(&versionMadeBy), sizeof(versionMadeBy));
    out.write(reinterpret_cast<char*>(&versionNeeded), sizeof(versionNeeded));
    out.write(reinterpret_cast<char*>(&diskNumberZip64), sizeof(diskNumberZip64));
    out.write(reinterpret_cast<char*>(&centralDirectoryDiskNumberZip64), sizeof(centralDirectoryDiskNumberZip64));
    out.write(reinterpret_cast<char*>(&totalEntriesZip64), sizeof(totalEntriesZip64));
    out.write(reinterpret_cast<char*>(&totalEntriesCentralDirZip64), sizeof(totalEntriesCentralDirZip64));
    out.write(reinterpret_cast<char*>(&sizeOfCentralDirectoryZip64), sizeof(sizeOfCentralDirectoryZip64));
    out.write(reinterpret_cast<char*>(&offsetOfCentralDirectoryZip64), sizeof(offsetOfCentralDirectoryZip64));

    uint32_t zip64EocdLocatorSignature = 0x07064b50;
    uint32_t zip64EocdDiskNumber = 0;
    uint64_t zip64EocdOffset = centralDirectoryOffset + centralDirectorySize;
    uint32_t totalDisks = 1;

    out.write(reinterpret_cast<char*>(&zip64EocdLocatorSignature), sizeof(zip64EocdLocatorSignature));
    out.write(reinterpret_cast<char*>(&zip64EocdDiskNumber), sizeof(zip64EocdDiskNumber));
    out.write(reinterpret_cast<char*>(&zip64EocdOffset), sizeof(zip64EocdOffset));
    out.write(reinterpret_cast<char*>(&totalDisks), sizeof(totalDisks));
}

int main() {
    const std::string filename = "zeros.txt";
    const uint64_t uncompressedSize = pow(1024, 1);
    const uint32_t compressedSize = 0;
    const uint32_t crc = crc32(0L, Z_NULL, 0);
    const uint32_t localHeaderOffset = 0;

    std::ofstream zipFile("zeros.zip", std::ios::binary);

    std::ostringstream localFileHeaderStream;
    writeLocalFileHeader(localFileHeaderStream, filename, compressedSize, uncompressedSize, crc);
    auto localFileHeader = localFileHeaderStream.str();
    zipFile.write(localFileHeader.c_str(), localFileHeader.size());

    std::ostringstream centralDirectoryHeaderStream;
    writeCentralDirectoryHeader(centralDirectoryHeaderStream, filename, compressedSize, uncompressedSize, crc, localHeaderOffset);
    auto centralDirectoryHeader = centralDirectoryHeaderStream.str();
    zipFile.write(centralDirectoryHeader.c_str(), centralDirectoryHeader.size());

    std::ostringstream endOfCentralDirectoryRecordStream;
    writeEndOfCentralDirectoryRecord(endOfCentralDirectoryRecordStream, centralDirectoryHeader.size(), localFileHeader.size() + compressedSize);
    auto endOfCentralDirectoryRecord = endOfCentralDirectoryRecordStream.str();
    zipFile.write(endOfCentralDirectoryRecord.c_str(), endOfCentralDirectoryRecord.size());

    zipFile.close();
    std::cout << "ZIP file 'zeros.zip' created successfully." << std::endl;

    return 0;
}
