# coding: utf-8
# GPS
#

class GPS

  DEFAULTmode = 0
  RMSmode     = 1

  #電源ON (松江高専ボードのみ)
  def power_on()
    gps_pw = GPIO.new(5, GPIO::OUT)
    gps_pw.write(0)
  end

  #電源OFF (松江高専ボードのみ)
  def power_off()
    gps_pw = GPIO.new(5, GPIO::OUT)
    gps_pw.write(1)
  end

  #初期化
  def initialize( uart, mode = 0)
    @uart = uart
    sleep 5

    #出力モードの切替え
    if mode == GPS::DEFAULTmode
      @uart.write("$PMTK314,-1*04\r\n")
    elsif mode == GPS::RMSmode
      @uart.write("$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n")
    end
  end
  
  #データ取得とステータス判断
  def is_ready?
    # 入力データをclear_tx_bufferで消去する
    @uart.clear_tx_buffer
    
    # 入力データが来るのを待つ. 2 秒間
    sleep 2
    
    # データ取得・表示．最終行を取得してカンマ区切りで配列化
    line = (@uart.read_nonblock(4096).split('$').pop).split(',')

    # ステータスが OK なら値をセットしておく.
    # if @line[2] == "A" AND @line[13].size == 14
    p line
    if line[2] == "A" 
      p "ok"
      year= "20#{line[9][4]}#{line[9][5]}"
      year2 = "#{line[9][4]}#{line[9][5]}"
      mon   = "#{line[9][2]}#{line[9][3]}"
      day   = "#{line[9][0]}#{line[9][1]}"
      hour  = "#{line[1][0]}#{line[1][1]}"
      min   = "#{line[1][2]}#{line[1][3]}"
      sec   = "#{line[1][4]}#{line[1][5]}"
      @datetime = [ year, year2, mon, day, hour, min, sec ]
      
      lat   = line[3]
      latDeg  = "#{lat[0]}#{lat[1]}".to_f
      latMin  = "#{lat[2]}#{lat[3]}#{lat[4]}#{lat[5]}#{lat[6]}#{lat[7]}#{lat[8]}}".to_f

      lng   = line[5]
      lngDeg  = "#{lng[0]}#{lng[1]}#{lng[2]}".to_f 
      lngMin  = "#{lng[3]}#{lng[4]}#{lng[5]}#{lng[6]}#{lng[7]}#{lng[8]}#{lng[9]}}".to_f

      latSign = line[4]
      lngSign = line[6]
      @position = [lat, latDeg, latMin, latSign, lng, lngDeg, lngMin, lngSign]
      
      return true
    else
      p "NG"
      # 返り値
      return false
    end
  end
  
  # 緯度 (latitude)．文字列を返す．フォーマット: dddmm.mmmm 
  def lat
    return @position[0]
  end

  # 緯度 (latitude)．数字の配列を返す
  def latDegMin
    return [@position[1], @position[2]]
  end
  
  # 緯度は北緯?
  def latNorth?
    return @position[3] == "N"
  end

  # 経度 (longitude)．文字列を返す．フォーマット: dddmm.mmmm 
  def lng
    return @position[4]
  end

  # 経度 (longitude)．数字の配列を返す
  def lngDegMin
    return [@position[5], @position[6]]
  end  

  # 経度は東経?
  def lngEast?
    return @position[7] == "E"
  end

  #緯度経度をまとめて配列で戻す
  def pos
    return [@position[1], @position[2], @position[5], @position[6]]
  end

  # 日付 (UTC)
  def year
    return @datetime[0].to_i
  end

  def year2
    return @datetime[1].to_i
  end

  def mon
    return @datetime[2].to_i
  end

  def day
    return @datetime[3].to_i
  end

  def hour
    return @datetime[4].to_i
  end

  def min
    return @datetime[5].to_i
  end    

  def sec
    return @datetime[6].to_i
  end

  def datetime
    return [@datetime[1].to_i, @datetime[2].to_i, @datetime[3].to_i, @datetime[4].to_i, @datetime[5].to_i, @datetime[6].to_i]
  end  

  def str_date
    return "#{@datetime[1]}-#{@datetime[2]}-#{@datetime[3]}"
  end

  def str_time
    return "#{@datetime[4]}:#{@datetime[5]}:#{@datetime[6]}"
  end
  
  def str_datetime
    return "#{@datetime[0]}#{@datetime[2]}#{@datetime[3]}#{@datetime[4]}#{@datetime[5]}#{@datetime[6]}"
  end

  
end
