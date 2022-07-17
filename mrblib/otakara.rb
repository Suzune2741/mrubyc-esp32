# coding: utf-8
# 宝探し用
#
class TreasureHunt

  def initialize( wlan )
    @wlan = wlan
    @store = []
  end

  # 宝の位置
  def pos1
    return [35,29.8100,133,1.5250]
  end

  def pos2
    return [35,29.7431,133,1.5960]
  end

  def pos3
    return [35,29.8635,133,1.5397]
  end

  def pos4
    return [35,29.7880,133,1.5800]
  end

  def pos5
    return [35,29.8191,133,1.6020]
  end

  def pos6
    return [35,29.8160,133,1.5628]
  end

  def pos7
    return [35,29.8490,133,1.5792]
  end

  def pos8
    return [35,29.7717,133,1.5980]
  end  
  
  # 宝の距離 [m] 計算．北緯 35 度での値で計算!!  入力された緯度経度を利用．
  def calcDist( pos )
    latDist = (
                ( @latDeg - pos[0]).abs * 60 
              + ( @latMin - pos[1]).abs
              ) * 1521

    lngDist = (
                ( @lngDeg - pos[2]).abs * 60 
              + ( @lngMin - pos[3]).abs
              ) * 1849
    @dist = Math.sqrt(latDist * latDist + lngDist * lngDist)
  end
  
  def dist
    return @dist
  end

  def send( host, datetime, lat, lng )
    # データ保管 
    @urls.push( "https://pluto.epi.it.matsue-ct.jp/gps/monitoring.php?hostname=#{host}&time=#{datetime}&lat=#{lat}&lng=#{lng}&utc=1" )
    
    # ネットワークへ送る
    if @wlan.connected?
      @urls.each do |url|
        @wlan.access( url )
      end
      #配列初期化
      @urls = []
    end
  end
  
end
