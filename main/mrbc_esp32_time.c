/*! @file
  @brief
  mruby/c TIME class for ESP32
*/

#include "mrbc_esp32_time.h"
#include "esp_sntp.h"
#include "esp_log.h"

static char* TAG = "Time";

typedef struct TIME_HANDLE {
  struct tm timeinfo;
  int   millis;
} TIME_HANDLE;

const char* timezone = CONFIG_MRUBYC_TIMEZONE;  // make menuconfig


void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Notification of a time synchronization event");
    ESP_LOGI(TAG, "Synced time: %ld.%06ld", (long)tv->tv_sec, (long)tv->tv_usec);
}

static void
mrbc_esp32_time_mktime(mrb_vm* vm, mrb_value* v, int argc)
{
  if (v[1].tt == MRBC_TT_INTEGER) {
    if (argc < 6) {
      ESP_LOGE(TAG, "Argument error: Manual set needs 6 integers (Y,M,D,h,m,s)");
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
  }else{
    ESP_LOGE(TAG, "Manual time set fail");
  }
}

static void
mrbc_esp32_time_sync_ntp(mrb_vm* vm, mrb_value* v, int argc)
{
  if (argc == 0 || v[1].tt == MRBC_TT_STRING) {
    char *server;
    
    if (argc == 0) {
      server = "ntp.nict.jp"; // デフォルト
    } else {
      server = (char *)GET_STRING_ARG(1);
    }

    ESP_LOGI(TAG, "Initializing SNTP with server: %s", server);   
    
    // 二重初期化防止
    if (esp_sntp_enabled()) {
      esp_sntp_stop();
    }

    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, server);
    esp_sntp_init();

    int retry = 0;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < 10) {
      ESP_LOGI(TAG, "Waiting for NTP sync... (%d/10)", retry);
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    // 同期完了後にタイムゾーンを再適用
    setenv("TZ", timezone, 1);
    tzset();

    ESP_LOGI(TAG, "NTP time set success");
  }else{
    ESP_LOGE(TAG, "NTP time set fail");
  }
}

//Time.now
//
static void
mrbc_esp32_time_now(mrb_vm* vm, mrb_value* v, int argc)
{
  if (v[0].tt != MRBC_TT_OBJECT){
    //インスタンス作成 
    v[0] = mrbc_instance_new(vm, v[0].cls, sizeof(TIME_HANDLE)); 
    mrbc_instance_call_initialize( vm, v, argc );
  }
  
  // 現在のシステム時刻(UNIXTIME)を取得
  struct timeval tv;
  gettimeofday(&tv, NULL);

  // インスタンスの保持しているメモリ領域へ直接書き込む
  TIME_HANDLE *hndl = (TIME_HANDLE *)(v[0].instance->data);  
  localtime_r(&tv.tv_sec, &hndl->timeinfo);
  hndl->millis = tv.tv_usec / 1000; 
}


static void
mrbc_esp32_time_date(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d", (hndl.timeinfo.tm_year + 1900) % 2000, hndl.timeinfo.tm_mon + 1, hndl.timeinfo.tm_mday );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_time(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d:%02d:%02d", hndl.timeinfo.tm_hour, hndl.timeinfo.tm_min, hndl.timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_datetime(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", (hndl.timeinfo.tm_year + 1900) % 2000, hndl.timeinfo.tm_mon + 1, hndl.timeinfo.tm_mday, hndl.timeinfo.tm_hour, hndl.timeinfo.tm_min, hndl.timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}

static void
mrbc_esp32_time_year(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( (hndl.timeinfo.tm_year + 1900) % 2000 );
}
static void
mrbc_esp32_time_mon(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_mon + 1 );
}
static void
mrbc_esp32_time_mday(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_mday );
}
static void
mrbc_esp32_time_wday(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_wday );
}
static void
mrbc_esp32_time_hour(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_hour );
}
static void
mrbc_esp32_time_min(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_min );
}
static void
mrbc_esp32_time_sec(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_INT_RETURN( hndl.timeinfo.tm_sec );
}
static void
mrbc_esp32_time_msec(mrb_vm* vm, mrb_value* v, int argc)
{
  TIME_HANDLE hndl = *((TIME_HANDLE *)(v[0].instance->data));
  SET_FLOAT_RETURN( hndl.timeinfo.tm_sec + hndl.millis / 1000.0 );
}

/*! クラス定義処理を記述した関数

  @param vm mruby/c VM
*/
void
mrbc_esp32_time_gem_init(struct VM* vm)
{
  mrbc_class *time = mrbc_define_class(0, "Time", 0);

  mrbc_define_method(0, time, "mktime",     mrbc_esp32_time_mktime);
  mrbc_define_method(0, time, "sync_ntp",   mrbc_esp32_time_sync_ntp);
  mrbc_define_method(0, time, "now",        mrbc_esp32_time_now);  
  mrbc_define_method(0, time, "datetime",   mrbc_esp32_time_datetime);
  mrbc_define_method(0, time, "date",       mrbc_esp32_time_date);
  mrbc_define_method(0, time, "time",       mrbc_esp32_time_time);
  mrbc_define_method(0, time, "year",       mrbc_esp32_time_year);
  mrbc_define_method(0, time, "mon",        mrbc_esp32_time_mon);
  mrbc_define_method(0, time, "mday",       mrbc_esp32_time_mday);
  mrbc_define_method(0, time, "wday",       mrbc_esp32_time_wday);
  mrbc_define_method(0, time, "hour",       mrbc_esp32_time_hour);
  mrbc_define_method(0, time, "min",        mrbc_esp32_time_min);
  mrbc_define_method(0, time, "sec",        mrbc_esp32_time_sec);
  mrbc_define_method(0, time, "msec",       mrbc_esp32_time_msec);
}
