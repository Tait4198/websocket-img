#include "JsonConvert.h"
#include <cstring>

using namespace Json;

bool JsonConvert::convert(const std::string &jsonStr, Json::Value *jsonValue) {
    JSONCPP_STRING errs;
    bool success = reader->parse(jsonStr.c_str(), jsonStr.c_str() + strlen(jsonStr.c_str()), jsonValue, &errs);
    return success && errs.empty();
}

JsonConvert::JsonConvert() {
    Json::CharReaderBuilder b;
    Json::CharReader *charReader(b.newCharReader());
    this->reader = charReader;
}

JsonConvert::~JsonConvert() {
    delete this->reader;
}