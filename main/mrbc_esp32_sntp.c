/*! @file
  @brief
  mruby/c SNTP class for ESP32
*/

#include "mrbc_esp32_sntp.h"
#include "esp_sntp.h"
#include "esp_log.h"

static char* TAG = "SNTP";




static void
mrbc_esp32_sntp_new(mrb_vm* vm, mrb_value* v, int argc)
{
  v[0] = mrbc_instance_new(vm, v[0].cls, sizeof(struct tm));

  //initialize を call
  mrbc_instance_call_initialize( vm, v, argc );

  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait
}

//void time_sync_notification_cb(struct timeval *tv) {
//    ESP_LOGI(TAG, "Notification of a time synchronization event");
//    ESP_LOGI(TAG, "Synced time: %ld.%06ld", (long)tv->tv_sec, (long)tv->tv_usec);
//}

static void
mrbc_esp32_sntp_initialize(mrb_vm* vm, mrb_value* v, int argc)
{
  ESP_LOGI(TAG, "Initializing SNTP");

  struct tm timeinfo = { 0 };
  time_t now = 0;
  int retry  = 0;
  const int retry_count = 10;

  //  sntp_set_time_sync_notification_cb(time_sync_notification_cb);
  
  // set time
  esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
  esp_sntp_setservername(0, "ntp.nict.jp");
  esp_sntp_init();
  
  // wait for time to be set
  while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
  
  //UNIXTIME の取得
  time(&now);

  int64_t time_us0 =  (int64_t)now;
  printf("Current s: %lld\n", time_us0);
  
  //JST に設定
  setenv("TZ", "JST-9", 1);
  tzset();

  //tm 構造体に格納
  localtime_r(&now, &timeinfo);

  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  /*
  int64_t time_us = (int64_t)tv_now.tv_sec * 1000000L + (int64_t)tv_now.tv_usec;
  printf("Current us: %lld\n", time_us);

  int64_t time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);

  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);

  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);
  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);
  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);
  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);
  vTaskDelay(100 / portTICK_PERIOD_MS);  //wait

  gettimeofday(&tv_now, NULL);
  time_ms = (int64_t)tv_now.tv_sec * 1000L + (int64_t)tv_now.tv_usec / 1000;
  printf("Current ms: %lld\n", time_ms);
  */
  
  //保管
  *((struct tm *)(v[0].instance->data)) = timeinfo;  
}

static void
mrbc_esp32_sntp_read(mrb_vm* vm, mrb_value* v, int argc)
{
  time_t now;
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));

  // 現在のシステム時刻(UNIXTIME)を取得
  time(&now);

  // ローカルタイム(JST)に変換して struct tm に格納
  localtime_r(&now, &timeinfo);

  // インスタンスのデータを現在の時刻で上書き
  *((struct tm *)(v[0].instance->data)) = timeinfo;

}


static void
mrbc_esp32_sntp_date(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d", (timeinfo.tm_year + 1900) % 2000, timeinfo.tm_mon + 1, timeinfo.tm_mday );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}
static void
mrbc_esp32_sntp_time(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}
static void
mrbc_esp32_sntp_datetime(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  char * buf  = (char *)malloc(sizeof(char) * 1024);
  
  sprintf(buf, "%02d-%02d-%02d %02d:%02d:%02d", (timeinfo.tm_year + 1900) % 2000, timeinfo.tm_mon + 1, timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec );

  mrbc_value ret = mrbc_string_new_cstr(vm, buf);
  SET_RETURN( ret );
}
static void
mrbc_esp32_sntp_year(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( (timeinfo.tm_year + 1900) % 2000);
}
static void
mrbc_esp32_sntp_mon(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_mon + 1);
}
static void
mrbc_esp32_sntp_mday(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_mday);
}
static void
mrbc_esp32_sntp_wday(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_wday);
}
static void
mrbc_esp32_sntp_hour(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_hour);
}
static void
mrbc_esp32_sntp_min(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_min);
}
static void
mrbc_esp32_sntp_sec(mrb_vm* vm, mrb_value* v, int argc)
{
  struct tm timeinfo = *((struct tm *)(v[0].instance->data));
  SET_INT_RETURN( timeinfo.tm_sec);
}

/*! クラス定義処理を記述した関数

  @param vm mruby/c VM
*/
void
mrbc_esp32_sntp_gem_init(struct VM* vm)
{
  mrbc_class *sntp = mrbc_define_class(0, "SNTP", 0);
  
  mrbc_define_method(0, sntp, "new",        mrbc_esp32_sntp_new);
  mrbc_define_method(0, sntp, "initialize", mrbc_esp32_sntp_initialize);
  mrbc_define_method(0, sntp, "read",  mrbc_esp32_sntp_read);  
  mrbc_define_method(0, sntp, "datetime", mrbc_esp32_sntp_datetime);
  mrbc_define_method(0, sntp, "date",  mrbc_esp32_sntp_date);
  mrbc_define_method(0, sntp, "time",  mrbc_esp32_sntp_time);
  mrbc_define_method(0, sntp, "year",  mrbc_esp32_sntp_year);
  mrbc_define_method(0, sntp, "mon",   mrbc_esp32_sntp_mon);
  mrbc_define_method(0, sntp, "mday",  mrbc_esp32_sntp_mday);
  mrbc_define_method(0, sntp, "wday",  mrbc_esp32_sntp_wday);
  mrbc_define_method(0, sntp, "hour",  mrbc_esp32_sntp_hour);
  mrbc_define_method(0, sntp, "min",   mrbc_esp32_sntp_min);
  mrbc_define_method(0, sntp, "sec",   mrbc_esp32_sntp_sec);
}

