# coding: utf-8

class WLAN

  STA_IF = 0   #端末
  AP_IF  = 1   #アクセスポイント
  ACTIVE = true
  
  # 初期化
  def initialize( name, flag = false )

    if name == 'STA'
      puts "STA_IF mode is selected"

    else
      puts "Sorry, AP_IF is not supported"
      stop
    end

    if flag == true 
      wifi_init()
    end
  end

  # スキャン
  def scan()
    wifi_scan()
  end

  # 有効化
  def active( flag )
    if flag == true 
      wifi_init()
    end
  end

  # 接続
  def connect(essid, password)
    wifi_setup_psk( essid, password )
    wifi_start()
  end

  # 接続確認
  def is_connected?()
    wifi_is_connected?()
  end

  # ifconfig
  def ifconfig()
    wifi_ifconfig( 'STA' )
  end

  # config
  def config( name )
    if name == "mac"
      wifi_mac( 'STA' )

    elsif name == "ip"
      wifi_ip( 'STA' )

    end
  end

  
  ###
  ### HTTP_client
  ###
  def invoke( url )
    httpclient_init( url )
    httpclient_invoke()
    httpclient_cleanup()
  end

  def access( url )
    httpclient_init( url )
    httpclient_invoke()
    httpclient_cleanup()
  end
 
end
