# coding: utf-8

# Minor の番号を全て集めると，暗号が解ける
# 数字暗号：https://cotobasearch.com/encrypt/number

#I2C 初期化
i2c = I2C.new(22, 21)

# LCD 初期化
lcd = AQM0802A.new(i2c)

# スイッチ初期化
sw34 = GPIO.new( 34, GPIO::IN, GPIO::PULL_UP)
sw35 = GPIO.new( 35, GPIO::IN, GPIO::PULL_UP)
sw18 = GPIO.new( 18, GPIO::IN, GPIO::PULL_UP)
sw19 = GPIO.new( 19, GPIO::IN, GPIO::PULL_UP)

# LED 初期化
led13 = GPIO.new( 13, GPIO::OUT )
led12 = GPIO.new( 12, GPIO::OUT )
led14 = GPIO.new( 14, GPIO::OUT )
led27 = GPIO.new( 27, GPIO::OUT )
led26 = GPIO.new( 26, GPIO::OUT )
led25 = GPIO.new( 25, GPIO::OUT )
led33 = GPIO.new( 33, GPIO::OUT )
led32 = GPIO.new( 32, GPIO::OUT )

# PWM 有効化
pwm15 = PWM.new( 15 )

# iBeacon 初期化
ibeacon = IBeacon.new

loop do
  info = ibeacon.get
#  puts "*** Major: #{ibeacon.major}   #{info['Major']}"
#  puts "*** Minor: #{ibeacon.minor}   #{info['Minor']}"
#  puts "*** RSSI:  #{ibeacon.rssi}    #{info['RSSI']}"
#  puts "*** Dist: #{ibeacon.dist} m   #{info['Dist']} m"

  if ibeacon.major == 11111

    # 3m を切っていれば LCD 表示. LED 4 つ点灯
    if ibeacon.dist < 3.0
      lcd.clear
      lcd.cursor(0, 0)
      lcd.write_string( ibeacon.minor.to_s )
      lcd.cursor(0, 1)
      lcd.write_string( ibeacon.dist.to_s )
      
      led13.on
      led14.on
      led12.on
      led27.on
      
      # 1m を切っていれば LED, ブザー
      if ibeacon.dist < 1.0

        if sw19.read == 1
          p "PWM ON"
#        pwm15.freq( 440 )
#        pwm15.duty(512)
        else
          p "PWM OFF"
#        pwm15.freq( 440 )
#        pwm15.duty(0)
        end

        led26.on
        led25.on
        led33.on
        led32.on
      end

      sleep 5
      
    # 遠ければ消灯
    else
      
      led13.off
      led14.off
      led12.off
      led27.off
      led26.off
      led25.off
      led33.off
      led32.off            
    end
  end
    
  sleep 0.05
end





=begin
# スイッチ初期化
sw34 = GPIO.new( 34, GPIO::IN, GPIO::PULL_UP)
sw35 = GPIO.new( 35, GPIO::IN, GPIO::PULL_UP)
sw18 = GPIO.new( 18, GPIO::IN, GPIO::PULL_UP)
sw19 = GPIO.new( 19, GPIO::IN, GPIO::PULL_UP)

# LED 初期化
led13 = GPIO.new( 13, GPIO::OUT )
led12 = GPIO.new( 12, GPIO::OUT )
led14 = GPIO.new( 14, GPIO::OUT )
led27 = GPIO.new( 27, GPIO::OUT )
led26 = GPIO.new( 26, GPIO::OUT )
led25 = GPIO.new( 25, GPIO::OUT )
led33 = GPIO.new( 33, GPIO::OUT )
led32 = GPIO.new( 32, GPIO::OUT )

# PWM 有効化
pwm15 = PWM.new( 15 )

#I2C 初期化
i2c = I2C.new(22, 21)

# CO2センサ初期化
scd30 = SCD30.new(i2c)

# LCD 初期化
lcd = AQM0802A.new(i2c)

# RTC 初期化. 時刻設定
rtc = RX8035SA.new(i2c)

# Wi-Fi 初期化
wlan = WLAN.new('STA', WLAN::ACTIVE)

# WiFiの接続
puts 'start connect....'
wlan.connect("SSID", "passwd")

# 時刻合わせ
sntp = SNTP.new

# 書き込み. 年(下2桁), 月, 日, 曜日, 時, 分, 秒
rtc.write( sntp.datetime )

# 観測
loop do
  if scd30.ready?

    puts "CO2         : #{scd30.co2} ppm"
    puts "Temperature : #{scd30.temp} C"
    puts "Humidity    : #{scd30.humi} %"

    # 音を鳴らす
    if scd30.co2 > 1500
      led13.on
      led14.on
      led12.on
      led27.on
      led26.on
      led25.on
      led33.on
      led32.on

      if sw19.read == 1
        pwm15.freq( 440 )
        pwm15.duty(512)
      else
        pwm15.freq( 440 )
        pwm15.duty(0)
      end

    elsif scd30.co2 > 1000
      led13.on
      led12.on
      led26.on
      led33.on
      led13.on
      
    else
      led13.off
      led14.off
      led12.off
      led27.off
      led26.off
      led25.off
      led33.off
      led32.off

    end

    # 時刻・緯度・経度を送る
    url = "https://pluto.epi.it.matsue-ct.jp/iotex2/monitoring3.php?hostname=hogehoge&time=#{rtc.str_datetime}&temp=#{scd30.temp}&co2=#{scd30.co2}"
    p url
    #    wlan.access( url )
    
    #表示
    lcd.clear
    lcd.cursor(0, 0)
    lcd.write_string( rtc.str_time.to_s )
    lcd.cursor(0, 1)
    lcd.write_string( scd30.co2.to_s )
  end

  sleep( 10 )
end

=end

=begin
# GPSの電源を入れる (高専ボードの場合に必要)
GPS.power_on

#UART 初期化
uart = UART.new(2, 9600)

# GPS 初期化
gps = GPS.new(uart, GPS::RMSmode)

#I2C 初期化
i2c = I2C.new(22, 21)

# LCD 初期化
lcd = AQM0802A.new(i2c)

# Wi-Fi 初期化
wlan = WLAN.new('STA', WLAN::ACTIVE)

# WiFiの接続
puts 'start connect....'
wlan.connect("SSID", "passwd")

# スイッチ初期化
sw34 = GPIO.new( 34, GPIO::IN, GPIO::PULL_UP)
sw35 = GPIO.new( 35, GPIO::IN, GPIO::PULL_UP)
sw18 = GPIO.new( 18, GPIO::IN, GPIO::PULL_UP)
sw19 = GPIO.new( 19, GPIO::IN, GPIO::PULL_UP)

# LED 初期化
led13 = GPIO.new( 13, GPIO::OUT )
led12 = GPIO.new( 12, GPIO::OUT )
led14 = GPIO.new( 14, GPIO::OUT )
led27 = GPIO.new( 27, GPIO::OUT )
led26 = GPIO.new( 26, GPIO::OUT )
led25 = GPIO.new( 25, GPIO::OUT )
led33 = GPIO.new( 33, GPIO::OUT )
led32 = GPIO.new( 32, GPIO::OUT )

# PWM 有効化
pwm15 = PWM.new( 15 )

# 宝探しクラスの初期化
takara = TreasureHunt.new( wlan )

while true
  if gps.dataReady?

    if sw34.read == 0 && sw35.read == 0 && sw18.read == 0
      led13.on
      takara.calcDist( takara.pos1 )
    end
    if sw34.read == 1 && sw35.read == 0 && sw18.read == 0
      led12.on
      takara.calcDist( takara.pos2 )
    end
    if sw34.read == 0 && sw35.read == 1 && sw18.read == 0
      led14.on
      takara.calcDist( takara.pos3 )
    end
    if sw34.read == 1 && sw35.read == 1 && sw18.read == 0
      led27.on
      takara.calcDist( takara.pos4 )
    end
    if sw34.read == 0 && sw35.read == 0 && sw18.read == 1
      led26.on
      takara.calcDist( takara.pos5 )
    end
    if sw34.read == 1 && sw35.read == 0 && sw18.read == 1
      led25.on
      takara.calcDist( takara.pos6 )
    end
    if sw34.read == 0 && sw35.read == 1 && sw18.read == 1
      led33.on
      takara.calcDist( takara.pos7 )
    end
    if sw34.read == 1 && sw35.read == 1 && sw18.read == 1
      led32.on
      takara.calcDist( takara.pos8 )
    end

    # 音を鳴らす
    if takara.dist < 10
      if sw19.read == 1
        pwm15.freq( 440 )
        pwm15.duty(512)
      else
        pwm15.freq( 440 )
        pwm15.duty(0)
      end
    end

    # 時刻・緯度・経度を送る
    takara.send( "hogehoge", gps.datetime, gps.lat, gps.lng )
    
    #表示
    lcd.clear
    lcd.cursor(0, 0)
    lcd.write_string("otakara")
    lcd.cursor(0, 1)
    lcd.write_string( gps.dist )

    #消灯
    led13.off
    led14.off
    led12.off
    led27.off
    led26.off
    led25.off
    led33.off
    led32.off

    sleep 5
 
  else

    #表示
    lcd.clear
    lcd.cursor(0, 0)
    lcd.write_string("xxxxxxxx")
  end
end

=end


=begin
# GPSの電源を入れる (高専ボードの場合に必要)
GPS.power_on

#UART 初期化
uart = UART.new(2, 9600)

# GPS 初期化
gps = GPS.new(uart, GPS::RMSmode)

while true
  if gps.dataReady?
    p gps.lng
    p gps.lngDegMin
    p gps.lat
    p gps.latDegMin
    p gps.str_date
    p gps.str_time
    p gps.str_datetime
    p gps.datetime
  else
    puts "wait...."    
  end
end
=end


=begin
ibeacon = IBeacon.new

loop do
  puts "rssi: #{ibeacon.rssi}"
  puts "Dist: #{ibeacon.dist} m"
  sleep 2
end
=end

=begin
#I2C 初期化
i2c = I2C.new(22, 21)

# CO2センサ初期化
scd30 = SCD30.new(i2c)

# 観測
loop do
  if scd30.dataReady
    var = scd30.read
    if var
      puts "CO2         : #{var[0]} ppm"
      puts "Temperature : #{var[1]} C"
      puts "Humidity    : #{var[2]} %"
    end
  end
  
  sleep( 0.5 )
end
=end

=begin
#I2C 初期化
i2c = I2C.new(22, 21)

# RTC 初期化. 時刻設定
rtc = RX8035SA.new(i2c)

# LCD 初期化
lcd = AQM0802A.new(i2c)

# Wi-Fi 初期化
wlan = WLAN.new('STA', WLAN::ACTIVE)

# WiFiの接続
puts 'start connect....'
wlan.connect("SSID", "passwd")

# 時刻合わせ
puts 'start SNTP...'
sntp = SNTP.new
p sntp.str_date, sntp.str_time, sntp.str_datetime, sntp.datetime

# 書き込み. 年(下2桁), 月, 日, 曜日, 時, 分, 秒
rtc.write( sntp.datetime )

# LCD に "Hello World" 表示
lcd.cursor(1, 0)
lcd.write_string("Hello!")
lcd.cursor(0, 1)
lcd.write_string("from ESP")
sleep(10)

while true
  
  # 時刻取得
  tt = rtc.read
  p rtc.read
  p rtc.datetime
  
  # 時刻表示
  lcd.cursor(0, 0)
  lcd.write_string( rtc.str_date )
  lcd.cursor(0, 1)
  lcd.write_string( rtc.str_time )
  
  p rtc.str_date, rtc.str_time, rtc.str_datetime
  
  # 待ち時間
  sleep 0.5
end
=end

=begin
#I2C 初期化
i2c = I2C.new(22, 21)

# RTC 初期化. 時刻設定
rtc = RX8035SA.new(i2c)
rtc.write([20, 3, 31, 1, 23, 59, 50]) #年(下2桁), 月, 日, 曜日, 時, 分, 秒

while true
  tt = rtc.read
  puts sprintf("%02d-%02d-%02d", tt[0], tt[1], tt[2])
  puts sprintf("%02d:%02d:%02d", tt[4], tt[5], tt[6])
  puts ""
  sleep 1.0
end
=end

=begin
#I2C 初期化
i2c = I2C.new(22, 21)

# LCD 初期化
lcd = AQM0802A.new(i2c)

# RTC 初期化. 時刻設定
rtc = RX8035SA.new(i2c)
rtc.write([20, 03, 31, 1, 23, 59, 50]) #年(下2桁), 月, 日, 曜日, 時, 分, 秒

# LCD に "Hello World" 表示
lcd.cursor(1, 0)           #カーソル位置を 0 行目の 1 文字目に
lcd.write_string("Hello!")

# i2c で直接送信
lcd.cursor(0, 1)           #カーソル位置を 1 行目の 0 文字目に
lcd.write_string("from ESP")

# Hello World 表示を行う
sleep(10)

while true
  tt = rtc.read
  
  puts sprintf("%02d-%02d-%02d", tt[0], tt[1], tt[2]) 
  puts sprintf("%02d:%02d:%02d", tt[4], tt[5], tt[6])
  puts ""
  
  lcd.clear
  lcd.cursor(0, 0)           #カーソル位置を 0 行目の 1 文字目に
  lcd.write_string( sprintf("%02d-%02d-%02d", tt[0], tt[1], tt[2]) )
  lcd.cursor(0, 1)           #カーソル位置を 1 行目の 0 文字目に
  lcd.write_string( sprintf("%02d:%02d:%02d", tt[4], tt[5], tt[6]) )

  sleep 1.0
end
=end
