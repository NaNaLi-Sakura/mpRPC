#include "mprpcconfig.h"
#include "_violet.h"

//加载解析配置文件
void MprcpConfig::LoadConfigFile(const char* config_file)
{
  //打开文件
  vio::cifile ifile;
  if (ifile.open(config_file) == false) {
    std::cout << config_file << " is open failed." << std::endl;
    exit(EXIT_FAILURE);
  }

  //逐行读取并解析
  vio::ccmdstr cmdstr; //拆分字符串
  std::string strbuffer{}; //存放读取到的每一行
  std::string key{}; std::string value{}; //配置项 键 值
  while (ifile.readline(strbuffer)) {
    //删掉两边的空格
    vio::deletelrchr(strbuffer, ' ');
    //跳过注释"#"行和空行
    if ((strbuffer[0] == '#') || (strbuffer.empty())) continue;
    //解析配置项 删除拆分后字段前后的空格
    cmdstr.splittocmd(strbuffer, "=", true);
    if (cmdstr.size() != 2) continue; //跳过不合要求的行
    cmdstr.getvalue(0, key);
    cmdstr.getvalue(1, value);
    //储存配置项
    m_configMap.insert({key, value});
  }
}

//查询配置项信息
std::string MprcpConfig::Load(const std::string& key)
{
  try {
    return m_configMap.at(key);
  }
  catch (const std::out_of_range&) {
    return "";
  }
}
