密钥体系
    安全模块所使用的密钥有 master key、base key 和 work key、master key 是每个本地通讯节点
存在一个，其作用是对 base key 和 work key 进行加密保护，以便在本地通讯节点上存储，其本身也
是加密存储的；base key 和 work key 是本地通讯节点为它的每个对等的通讯节点设置的密钥。

IBsec 服务所需的环境变量
  #IBSEC控制文件
  IBSECCTRLFILE=IBSEC_CTRL
  #安全节点编码
  IBSECNODE=09610000
  #IBSEC的KEY文件
  IBSECKEYDB=IBSECKEYDB
  #IBSEC的主KEY文件
  IBSECMKEY=IBSECMKEY
  #IBSEC运行模式
  IBSECMODE=SERVER
