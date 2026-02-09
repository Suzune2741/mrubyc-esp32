/*! @file
  @brief
  mruby/c TIME class for ESP32
*/

#include "mrbc_esp32_time.h"
#include "esp_sntp.h"
#include "esp_log.h"

static char* TAG = "Time";

//グローバル変数
struct tm timeinfo = {0};
int   millis = 0;
char* ntpServer = "ntp.nict.jp";
const char* timezone = CONFIG_MRUBYC_TIMEZONE;  // make menuconfig


//void time_sync_notification_cb(struct timeval *tv) {
//    ESP_LOGI(TAG, "Notification of a time synchronization event");
//    ESP_LOGI(TAG, "Synced time: %ld.%06ld", (long)tv->tv_sec, (long)tv->tv_usec);
//}

/*
static void
mrbc_esp32_time_new(mrb_vm* vm, mrb_value* v, int argc)
{
  // --- 引数が Integer の場合 (mktime 処理) ---
  if (v[1].tt == mrbc_vt_integer) {
    if (argc < 6) {
      ESP_LOGE(TAG, "Argument error: mktime needs 6 integers");
      return;
    }
    struct tm tm_input = {0};
    tm_input.tm_year = GET_INT_ARG(1) - 1900;
    tm_input.tm_mon  = GET_INT_ARG(2) - 1;
    tm_input.tm_mday = GET_INT_ARG(3);
    tm_input.tm_hour = GET_INT_ARG(4);
    tm_input.tm_min  = GET_INT_ARG(5);
    tm_input.tm_sec  = GET_INT_ARG(6);
    tm_input.tm_isdst = -1;

    setenv("TZ", timezone, 1);
    tzset();

    time_t t = mktime(&tm_input);
    if (t != (time_t)-1) {
      struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
      settimeofday(&tv, NULL);
      ESP_LOGI(TAG, "Manual time set success");
    }
  } 
  // --- 引数が String の場合 (NTP 処理) ---
  else if (v[1].tt == mrbc_vt_string) {
    char *server = (char *)GET_STRING_ARG(1);
    
    // 既に起動している場合は一旦停止して再設定
    if (esp_sntp_enabled()) {
      esp_sntp_stop();
    }
    
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, server);
    esp_sntp_init();

    int retry = 0;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < 10) {
      ESP_LOGI(TAG, "Waiting for NTP sync... (%d/10)", retry);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}
*/
//Time.mktime()
//
static void
mrbc_esp32_time_mktime(mrb_vm* vm, mrb_value* v, int argc)
{
  timeinfo.tm_year = GET_INT_ARG(1) - 1900; // 1900年からの経過年
  timeinfo.tm_mon = GET_INT_ARG(2) - 1;     // 月は 0〜11 で指定 (1月が0)
  timeinfo.tm_mday = GET_INT_ARG(3);
  timeinfo.tm_hour = GET_INT_ARG(4);
  timeinfo.tm_min = GET_INT_ARG(5);
  timeinfo.tm_sec = GET_INT_ARG(6);
  timeinfo.tm_isdst = -1;                   // 夏時間の有無（不明な場合は-1）
  
  //JST に設定
  setenv("TZ", timezone, 1);
  tzset();

  //UNIXTIME の作成
  time_t t = mktime(&timeinfo);
  
  if (t != (time_t)-1) {
    struct timeval tv = { .tv_sec = t, .tv_usec = 0 };
    settimeofday(&tv, NULL);
  }
}


//Time.ntp()
//
static void
mrbc_esp32_time_ntp(mrb_vm* vm, mrb_value* v, int argc)
{
  int retry  = 0;
  const int retry_count = 10;
  
  if (GET_STRING_ARG(1))
    ntpServer = (char*)GET_STRING_ARG(1);
  
  ESP_LOGI(TAG, "Initializing SNTP");
  //  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  
  // set time
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, ntpServer);
  esp_sntp_init();
  
  // wait for time to be set
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

//Time.now
//
static void
mrbc_esp32_time_now(mrb_vm* vm, mrb_value* v, int argc)
{
  struct timeval tv;
  
  // 現在のシステム時刻(UNIXTIME)を取得
  gettimeofday(&tv, NULL);

  //タイムゾーン
  setenv("TZ", timezone, 1);
  tzset();
  
  //tm 構造体に格納
  localtime_r(&tv.tv_sec, &timeinfo);

  //ミリ秒
  millis = tv.tv_usec / 1000; //マイクロ秒から変換
}


static void
mrbc_esp32_time_date(mrb_vm* vm, mrb_value* v, int argc)
{
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d", (timeinfo.tm_year + 1900) % 2000, timeinfo.tm_mon + 1, timeinfo.tm_mday );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_time(mrb_vm* vm, mrb_value* v, int argc)
{
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_datetime(mrb_vm* vm, mrb_value* v, int argc)
{
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", (timeinfo.tm_year + 1900) % 2000, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_year(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( (timeinfo.tm_year + 1900) % 2000 );
}
static void
mrbc_esp32_time_mon(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_mon + 1 );
}
static void
mrbc_esp32_time_mday(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_mday );
}
static void
mrbc_esp32_time_wday(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_wday );
}
static void
mrbc_esp32_time_hour(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_hour );
}
static void
mrbc_esp32_time_min(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_min );
}
static void
mrbc_esp32_time_sec(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_INT_RETURN( timeinfo.tm_sec );
}
static void
mrbc_esp32_time_msec(mrb_vm* vm, mrb_value* v, int argc)
{
  SET_FLOAT_RETURN( timeinfo.tm_sec + millis / 1000.0 );
}

/*! クラス定義処理を記述した関数

  @param vm mruby/c VM
*/
void
mrbc_esp32_time_gem_init(struct VM* vm)
{
  mrbc_class *time = mrbc_define_class(0, "Time", 0);

  mrbc_define_method(0, time, "mktime",   mrbc_esp32_time_mktime);
  mrbc_define_method(0, time, "ntp",      mrbc_esp32_time_ntp);
  mrbc_define_method(0, time, "now",      mrbc_esp32_time_now);  
  mrbc_define_method(0, time, "datetime", mrbc_esp32_time_datetime);
  mrbc_define_method(0, time, "date",     mrbc_esp32_time_date);
  mrbc_define_method(0, time, "time",     mrbc_esp32_time_time);
  mrbc_define_method(0, time, "year",     mrbc_esp32_time_year);
  mrbc_define_method(0, time, "mon",      mrbc_esp32_time_mon);
  mrbc_define_method(0, time, "mday",     mrbc_esp32_time_mday);
  mrbc_define_method(0, time, "wday",     mrbc_esp32_time_wday);
  mrbc_define_method(0, time, "hour",     mrbc_esp32_time_hour);
  mrbc_define_method(0, time, "min",      mrbc_esp32_time_min);
  mrbc_define_method(0, time, "sec",      mrbc_esp32_time_sec);
  mrbc_define_method(0, time, "msec",     mrbc_esp32_time_msec);
}
