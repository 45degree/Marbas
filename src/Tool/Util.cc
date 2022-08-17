#include "Tool/Util.hpp"

#include <glog/logging.h>
#include <openssl/sha.h>

#include <fstream>

namespace Marbas {

String
Sha256(const String& content) {
  char buf[3] = {0, 0, 0};
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, content.c_str(), content.size());
  SHA256_Final(hash, &sha256);
  String NewString = "";
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(buf, "%02x", hash[i]);
    NewString = NewString + buf;
  }
  return NewString;
}

String
ReadFileSource(const Path& path) {
  String content;
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
    file.seekg(0, std::ios::beg);

    std::stringstream stringStream;
    stringStream << file.rdbuf();
    file.close();

    content = stringStream.str();
  } catch (const std::exception& e) {
    LOG(ERROR) << FORMAT("can't read the file {}, and cause an exception: {}", path, e.what());

    if (file.is_open()) {
      file.close();
    }

    throw e;
  }

  return content;
}

String
ReadFileFromBinary(const Path& path) {
  std::ifstream file;
  String content = "";
  file.open(path, std::ios_base::in | std::ios_base::binary);
  if (!file.is_open()) return content;

  // read the spri-v content
  try {
    file.seekg(0, std::ios::beg);
    file.seekg(0, std::ios::end);
    auto size = file.tellg();
    content.resize(size);
    file.seekg(0, std::ios::beg);
    file.read(content.data(), size);
  } catch (const std::exception& e) {
    file.close();
    throw std::exception(e);
  }
  file.close();

  return content;
}

void
WriteBinaryToFile(const Path& path, const Vector<uint32_t>& content) {
  std::ofstream file(path.c_str(), std::ios::out | std::ios::binary);

  try {
    file.seekp(0, std::ios::beg);
    file.write((char*)content.data(), content.size() * sizeof(content[0]));
    file.flush();
  } catch (const std::exception& e) {
    file.close();
    throw std::exception(e);
  }
  file.close();
  return;
}

}  // namespace Marbas
