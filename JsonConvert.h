#include <json/json.h>
#include <string>

class JsonConvert {
public:
    JsonConvert();

    ~JsonConvert();

    bool convert(const std::string& jsonStr, Json::Value *jsonValue);

private:
    Json::CharReader *reader;
};
