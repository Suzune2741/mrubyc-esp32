# coding: utf-8
class PWM

  # 定数
  # タイマーの精度とスピードモードは決め打ち
  # esp-idf/components/driver/include/driver/ledc.h 参照
  LEDC_TIMER_8_BIT = 8
  LEDC_HIGH_SPEED_MODE = 0
  LEDC_DEFAULT_FREQUENCY = 440
  
  # 初期化
  def initialize(pin, ch=0)
    @pin  = pin
    @ch = ch

    # 定義済みのチャンネルを取ってこれれば引数にチャンネルを与えなくてよくなるのだが.
    # 後日検討.
    
    # タイマーの初期化
    LEDC.timer_config(
      PWM::LEDC_TIMER_8_BIT,
      PWM::LEDC_DEFAULT_FREQUENCY,
      PWM::LEDC_HIGH_SPEED_MODE
    )
    
    # PWM の初期化
    LEDC.channel_config(
      @ch,
      @pin, 
      PWM::LEDC_HIGH_SPEED_MODE
    )
  end

  # デューティー比の設定
  def duty( duty )
    @duty = duty
    LEDC.set_duty(PWM::LEDC_HIGH_SPEED_MODE, @ch, @duty)
    LEDC.update_duty(PWM::LEDC_HIGH_SPEED_MODE, @ch)

    puts "Set Duty : #{@duty}"
  end

  # 周波数の設定
  def freq( freq )
    LEDC.timer_config(
      PWM::LEDC_TIMER_8_BIT,
      freq,
      PWM::LEDC_HIGH_SPEED_MODE
    )
    LEDC.set_duty(PWM::LEDC_HIGH_SPEED_MODE, @ch, @duty)
    LEDC.update_duty(PWM::LEDC_HIGH_SPEED_MODE, @ch)
    puts "Set Frequency : #{freq}"
    puts "Duty is already set: #{@duty}"
  end

end
