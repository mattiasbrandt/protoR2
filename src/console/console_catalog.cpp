// =============================================================================
// src/console/console_catalog.cpp
//
// Auto-generated from docs/action-registry.yaml by tools/generate_console_catalog.py
// DO NOT EDIT MANUALLY
//
// Operation Catalog - runtime table mapping operation names to descriptors,
// parameter schemas, availability metadata, and help text addressing.
// Help text (description, display_name, parameter schema, executor details)
// is stored in LittleFS and addressed by offset/length.
//
// Availability (available_on_board and available_in_build) is evaluated at
// compile-time via macros from include/config.h and include/board_capabilities.inc
// (ADR 0029): available_on_board is the entry's `board_capability:` macro and
// available_in_build its `build_flag:`, emitted by name rather than resolved
// here. A board that sets PA_CAP_DRIVE_BACKEND_HOVERBOARD=0 or
// PA_CAP_HOSTED_WIFI=0 flips those rows with no generator change and no
// regeneration.
// =============================================================================

#include "console_catalog.h"
#include "config.h"
#include <string.h>

// =============================================================================
// Alias Arrays (RC tokens mapped to operation names)
// =============================================================================

static const char* const g_aliases_drive_action_speed[] = { "drive_speed", NULL };
static const char* const g_aliases_drive_action_steer[] = { "drive_steer", NULL };
static const char* const g_aliases_drive_action_speed_preset_cycle[] = { "speed_preset_cycle", NULL };
static const char* const g_aliases_dome_action_set_speed[] = { "dome_speed", NULL };
static const char* const g_aliases_dome_action_marcduino_sequence[] = { "seq", NULL };
static const char* const g_aliases_dome_action_marcduino_command[] = { "cmd", NULL };
static const char* const g_aliases_dome_action_dome_sequence[] = { "dome_seq", NULL };
static const char* const g_aliases_dome_action_droid_sequence_scream[] = { "droid_seq_scream", NULL };
static const char* const g_aliases_dome_action_droid_sequence_wave[] = { "droid_seq_wave", NULL };
static const char* const g_aliases_dome_action_droid_sequence_fast_wave[] = { "droid_seq_fast_wave", NULL };
static const char* const g_aliases_dome_action_droid_sequence_open_wave[] = { "droid_seq_open_wave", NULL };
static const char* const g_aliases_dome_action_droid_sequence_beep_cantina[] = { "droid_seq_beep_cantina", NULL };
static const char* const g_aliases_dome_action_droid_sequence_faint[] = { "droid_seq_faint", NULL };
static const char* const g_aliases_dome_action_droid_sequence_cantina[] = { "droid_seq_cantina", NULL };
static const char* const g_aliases_dome_action_droid_sequence_leia[] = { "droid_seq_leia", NULL };
static const char* const g_aliases_dome_action_droid_sequence_disco[] = { "droid_seq_disco", NULL };
static const char* const g_aliases_dome_action_droid_sequence_screams[] = { "droid_seq_screams", NULL };
static const char* const g_aliases_dome_action_droid_sequence_wiggle[] = { "droid_seq_wiggle", NULL };
static const char* const g_aliases_sound_action_random_general[] = { "sound_rand_general", NULL };
static const char* const g_aliases_sound_action_random_chatty[] = { "sound_rand_chatty", NULL };
static const char* const g_aliases_sound_action_random_happy[] = { "sound_rand_happy", NULL };
static const char* const g_aliases_sound_action_random_processing[] = { "sound_rand_processing", NULL };
static const char* const g_aliases_sound_action_random_sad[] = { "sound_rand_sad", NULL };
static const char* const g_aliases_sound_action_random_sentimental[] = { "sound_rand_sentimental", NULL };
static const char* const g_aliases_sound_action_random_humming[] = { "sound_rand_humming", NULL };
static const char* const g_aliases_sound_action_random_scream[] = { "sound_rand_scream", NULL };
static const char* const g_aliases_sound_action_random_surprised[] = { "sound_rand_surprised", NULL };
static const char* const g_aliases_sound_action_random_alert[] = { "sound_rand_alert", NULL };
static const char* const g_aliases_sound_action_random_snarky[] = { "sound_rand_snarky", NULL };
static const char* const g_aliases_sound_action_random_whistle[] = { "sound_rand_whistle", NULL };
static const char* const g_aliases_sound_action_play_next[] = { "sound_next", NULL };
static const char* const g_aliases_sound_action_play_previous[] = { "sound_previous", NULL };
static const char* const g_aliases_servo_action_toggle_arm1[] = { "arm1_toggle", NULL };
static const char* const g_aliases_servo_action_toggle_arm2[] = { "arm2_toggle", NULL };
static const char* const g_aliases_servo_action_toggle_aux1[] = { "aux1_toggle", NULL };
static const char* const g_aliases_servo_action_toggle_aux2[] = { "aux2_toggle", NULL };
static const char* const g_aliases_servo_action_toggle_aux3[] = { "aux3_toggle", NULL };
static const char* const g_aliases_servo_action_puppet_part[] = { "puppet_part", NULL };
static const char* const g_aliases_system_action_set_mode[] = { "op_mode", NULL };
static const char* const g_aliases_system_action_estop[] = { "estop", NULL };
static const char* const g_aliases_system_action_sleep_toggle[] = { "sleep_toggle", NULL };

// Total alias arrays: 41

// =============================================================================
// Parameter Descriptors
// =============================================================================

static const char* const g_enum_drive_action_speed_preset_slow_preset[] = { "slow", NULL };
static const char* const g_enum_drive_action_speed_preset_normal_preset[] = { "normal", NULL };
static const char* const g_enum_drive_action_speed_preset_turbo_preset[] = { "turbo", NULL };
static const char* const g_enum_sound_api_play_banked_page[] = { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", NULL };
static const char* const g_enum_sound_config_track_assignments_key[] = { "scream", "faint", "leia", "cantina_s", "sw_theme", "imp_march", "cantina_l", "startup", "happy", "doodoo", "failure", "disco", "mahna", "inlove", "macho", "gangnam", "uptown", "celebr", "stayin", "harlem", "pbjtime", NULL };
static const char* const g_enum_sound_config_system_track_assignments_key[] = { "sys_boot", "sys_mode_n", "sys_mode_s", "sys_mode_t", "sys_drv_on", "sys_dome_on", "sys_net_down", NULL };
static const char* const g_enum_servo_action_open_target[] = { "both", NULL };
static const char* const g_enum_servo_action_close_target[] = { "both", NULL };
static const char* const g_enum_servo_action_release_target[] = { "both", NULL };
static const char* const g_enum_servo_action_stop_target[] = { "both", NULL };
static const char* const g_enum_aux_action_led_effect_effect[] = { "solid", "blink", "pulse", "off", NULL };
static const char* const g_enum_system_action_set_mood_mood[] = { "10", "11", "13", "14", NULL };
static const char* const g_enum_wifi_config_settings_mode[] = { "client", "standalone_ap", NULL };

// Total enum-value arrays: 13

static const ConsoleParamDescriptor g_params_drive_action_move[] = {
    {"speed", "int16", true, true, -1000.0, 1000.0, NULL, false, false},
    {"steer", "int16", true, true, -1000.0, 1000.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_drive_action_speed[] = {
    {"value", "float", true, true, -1.0, 1.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_drive_action_steer[] = {
    {"value", "float", true, true, -1.0, 1.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_drive_action_speed_preset_slow[] = {
    {"preset", "string", true, false, 0.0, 0.0, g_enum_drive_action_speed_preset_slow_preset, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_drive_action_speed_preset_normal[] = {
    {"preset", "string", true, false, 0.0, 0.0, g_enum_drive_action_speed_preset_normal_preset, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_drive_action_speed_preset_turbo[] = {
    {"preset", "string", true, false, 0.0, 0.0, g_enum_drive_action_speed_preset_turbo_preset, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_move[] = {
    {"speed", "float", true, true, -1.0, 1.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_api_get_sequence[] = {
    {"name", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_delete_sequence[] = {
    {"name", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_test_sequence[] = {
    {"name", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_pose_sequence[] = {
    {"name", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"t", "int32", true, true, 0.0, 2147483647.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_pin_sequence[] = {
    {"name", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"pinned", "bool", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_action_arm_take[] = {
    {"seq", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_dome_api_get_take_file[] = {
    {"owner", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"take", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_action_play_track[] = {
    {"track", "uint16", true, true, 1.0, 999.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_api_play_banked[] = {
    {"bank", "uint8", true, true, 1.0, 6.0, NULL, false, false},
    {"page", "string", true, false, 0.0, 0.0, g_enum_sound_api_play_banked_page, false, false},
    {"index", "uint16", true, true, 1.0, 65535.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_action_set_mood_map[] = {
    {"quiet", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"mid", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"full", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"awakeplus", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_action_set_category_range[] = {
    {"lo_key", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"hi_key", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"lo", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"hi", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_action_set_volume[] = {
    {"volume", "uint8", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_volume[] = {
    {"volume", "uint8", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_random_min[] = {
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_random_max[] = {
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_startup_track[] = {
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_boot_complete_track[] = {
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_network_down_track[] = {
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_track_assignments[] = {
    {"key", "string", true, false, 0.0, 0.0, g_enum_sound_config_track_assignments_key, false, false},
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_system_track_assignments[] = {
    {"key", "string", true, false, 0.0, 0.0, g_enum_sound_config_system_track_assignments_key, false, false},
    {"track", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_category_ranges[] = {
    {"lo_key", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"hi_key", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"lo", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"hi", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_sound_config_mood_category_map[] = {
    {"quiet", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"mid", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"full", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {"awakeplus", "uint16", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_open[] = {
    {"target", "string", true, false, 0.0, 0.0, g_enum_servo_action_open_target, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_close[] = {
    {"target", "string", true, false, 0.0, 0.0, g_enum_servo_action_close_target, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_set_position[] = {
    {"target", "string", true, false, 0.0, 0.0, NULL, false, true},
    {"position_us", "uint16", true, true, 500.0, 2500.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_nudge[] = {
    {"target", "string", true, false, 0.0, 0.0, NULL, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_travel[] = {
    {"target", "string", true, false, 0.0, 0.0, NULL, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_hold[] = {
    {"target", "string", true, false, 0.0, 0.0, NULL, false, true},
    {"position_us", "uint16", true, true, 500.0, 2500.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_release[] = {
    {"target", "string", true, false, 0.0, 0.0, g_enum_servo_action_release_target, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_servo_action_stop[] = {
    {"target", "string", true, false, 0.0, 0.0, g_enum_servo_action_stop_target, false, true},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_aux_action_led_color[] = {
    {"r", "uint8", true, true, 0.0, 255.0, NULL, false, false},
    {"g", "uint8", true, true, 0.0, 255.0, NULL, false, false},
    {"b", "uint8", true, true, 0.0, 255.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_aux_action_led_effect[] = {
    {"effect", "string", true, false, 0.0, 0.0, g_enum_aux_action_led_effect_effect, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_aux_config_led_count[] = {
    {"target", "string", true, false, 0.0, 0.0, NULL, false, true},
    {"value", "uint8", false, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_system_action_set_mood[] = {
    {"mood", "uint8", true, false, 0.0, 0.0, g_enum_system_action_set_mood_mood, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_wifi_config_settings[] = {
    {"mode", "string", false, false, 0.0, 0.0, g_enum_wifi_config_settings_mode, false, false},
    {"sta-ssid", "string", false, false, 0.0, 0.0, NULL, false, false},
    {"ap-ssid", "string", false, false, 0.0, 0.0, NULL, false, false},
    {"sta-password", "string", false, false, 0.0, 0.0, NULL, true, false},
    {"ap-password", "string", false, false, 0.0, 0.0, NULL, true, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_system_action_set_identity[] = {
    {"droidName", "string", true, false, 0.0, 0.0, NULL, false, false},
    {"mdnsUseName", "bool", false, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

static const ConsoleParamDescriptor g_params_rc_action_test_bindable[] = {
    {"token", "string", true, false, 0.0, 0.0, NULL, false, false},
    {NULL, NULL, false, false, 0.0, 0.0, NULL, false, false}  // terminator
};

// =============================================================================
// Status Query Field Lists (API JSON keys, verbatim)
// =============================================================================

static const char* const g_fields_dome_status_current[] = { "domeTargetSpeed", "domeEnabled", "domeBearing", "domeBearingDeg", NULL };
static const char* const g_fields_dome_api_get_sequence_last_run[] = { "valid", "name", "source", "outcome", "running", "reason", "startMs", "endMs", NULL };
static const char* const g_fields_sound_api_get_catalog[] = { "ready", NULL };
static const char* const g_fields_sound_api_get_mood_map[] = { "quiet", "mid", "full", "awakeplus", NULL };
static const char* const g_fields_sound_status_current[] = { "driver", "output", "capabilities", "link_ok", "active", "play_state", "device", "total_tracks", "current_track", "missing_track", "rx_status", "rx_detail", NULL };
static const char* const g_fields_system_status_health[] = { "estop", "sbusSignalLost", "sbusHwFailsafe", "webControlEnabled", "wifiConnected", "wifiClientConnected", "littleFsReady", "heapFree", "heapMin", "heapLargestBlock", "heapLargest8bit", "allocBlocks", "httpSocketsOpen", "sseClients", "wifiRssi", "uptimeMs", "resetReason", NULL };
static const char* const g_fields_system_status_wifi[] = { "apSsid", "apIp", "staEnabled", "staConnected", "staIp", "staSsid", "wifiRssi", "networkRecovery", NULL };
static const char* const g_fields_system_status_hosted_link[] = { "phase", "terminal", "transportFailureCount", "transportUpEventCount", "attemptCount", "totalAttemptCount", "recoveredCount", "lastFailureAtMs", "lastAttemptAtMs", "degradedAtMs", "livenessSource", "livenessMissCount", "heartbeatCount", "lastHeartbeatNumber", "livenessAgeMs", "lastAttemptInit", "lastAttemptRefusal", "lastAttemptHeartbeatConfig", "lastAttemptWifiGetMode", "lastAttemptHeartbeatSeen", NULL };
static const char* const g_fields_dome_status_serial_link[] = { "active", "heartbeatRx", "heartbeatTx", NULL };
static const char* const g_fields_system_api_get_identity[] = { "droidName", "mdnsUseName", NULL };
static const char* const g_fields_system_api_get_validation[] = { "updatedMs", "drive", "domeLink", "audio", "rc", NULL };
static const char* const g_fields_rc_status_snapshot[] = { "mode", "sbus1", "sbus2", NULL };

// Total field-name arrays: 12

// =============================================================================
// Complete Operation Catalog
// =============================================================================

static const ConsoleCatalogEntry g_catalogEntries[] = {
    {
        "drive.action.move",
        "action",
        NULL,  // aliases
        g_params_drive_action_move,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        true,  // safety_critical
        0,  // help_offset
        125,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.speed",
        "action",
        g_aliases_drive_action_speed,  // aliases
        g_params_drive_action_speed,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        126,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.steer",
        "action",
        g_aliases_drive_action_steer,  // aliases
        g_params_drive_action_steer,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        234,  // help_offset
        105,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.speed-preset-slow",
        "action",
        NULL,  // aliases
        g_params_drive_action_speed_preset_slow,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        340,  // help_offset
        121,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.speed-preset-normal",
        "action",
        NULL,  // aliases
        g_params_drive_action_speed_preset_normal,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        462,  // help_offset
        127,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.speed-preset-turbo",
        "action",
        NULL,  // aliases
        g_params_drive_action_speed_preset_turbo,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        590,  // help_offset
        124,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.action.speed-preset-cycle",
        "action",
        g_aliases_drive_action_speed_preset_cycle,  // aliases
        NULL,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        715,  // help_offset
        136,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.status.current",
        "status",
        NULL,  // aliases
        NULL,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        852,  // help_offset
        102,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.event.failsafe-triggered",
        "event",
        NULL,  // aliases
        NULL,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        true,  // safety_critical
        955,  // help_offset
        116,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "drive.config.speed-limit",
        "config",
        NULL,  // aliases
        NULL,
        PA_CAP_DRIVE_BACKEND_HOVERBOARD,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1072,  // help_offset
        129,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.set-speed",
        "action",
        g_aliases_dome_action_set_speed,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1202,  // help_offset
        89,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.send-command",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1292,  // help_offset
        99,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.marcduino-sequence",
        "action",
        g_aliases_dome_action_marcduino_sequence,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1392,  // help_offset
        125,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.marcduino-command",
        "action",
        g_aliases_dome_action_marcduino_command,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1518,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.dome-sequence",
        "action",
        g_aliases_dome_action_dome_sequence,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1629,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-scream",
        "action",
        g_aliases_dome_action_droid_sequence_scream,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1762,  // help_offset
        102,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-wave",
        "action",
        g_aliases_dome_action_droid_sequence_wave,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1865,  // help_offset
        93,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-fast-wave",
        "action",
        g_aliases_dome_action_droid_sequence_fast_wave,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        1959,  // help_offset
        103,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-open-wave",
        "action",
        g_aliases_dome_action_droid_sequence_open_wave,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2063,  // help_offset
        104,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-beep-cantina",
        "action",
        g_aliases_dome_action_droid_sequence_beep_cantina,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2168,  // help_offset
        128,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-faint",
        "action",
        g_aliases_dome_action_droid_sequence_faint,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2297,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-cantina",
        "action",
        g_aliases_dome_action_droid_sequence_cantina,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2405,  // help_offset
        123,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-leia",
        "action",
        g_aliases_dome_action_droid_sequence_leia,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2529,  // help_offset
        106,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-disco",
        "action",
        g_aliases_dome_action_droid_sequence_disco,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2636,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-screams",
        "action",
        g_aliases_dome_action_droid_sequence_screams,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2749,  // help_offset
        114,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.droid-sequence-wiggle",
        "action",
        g_aliases_dome_action_droid_sequence_wiggle,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2864,  // help_offset
        103,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.get-layout",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        2968,  // help_offset
        137,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Dashboard",  // console_page
    },
    {
        "dome.action.sequence-stop",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3106,  // help_offset
        128,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.config.stand-down",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3235,  // help_offset
        167,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-scream",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3403,  // help_offset
        115,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-happy",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3519,  // help_offset
        116,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-overload",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3636,  // help_offset
        119,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-alarm",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3756,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-vader",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3869,  // help_offset
        99,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-rockmarch",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        3969,  // help_offset
        122,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-leia",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4092,  // help_offset
        95,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-cantina",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4188,  // help_offset
        96,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-heart",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4285,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-hello",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4378,  // help_offset
        88,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.event.cue-reset",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4467,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.status.current",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4580,  // help_offset
        155,  // help_length
        g_fields_dome_status_current,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.move",
        "action",
        NULL,  // aliases
        g_params_dome_action_move,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4736,  // help_offset
        149,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.front-is-here",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        4886,  // help_offset
        306,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.go-home",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5193,  // help_offset
        242,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.list-sequences",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5436,  // help_offset
        151,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.list-builtin-sequences",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5588,  // help_offset
        126,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.get-sequence",
        "action",
        NULL,  // aliases
        g_params_dome_api_get_sequence,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5715,  // help_offset
        119,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "dome.action.save-sequence",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5835,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "dome.action.delete-sequence",
        "action",
        NULL,  // aliases
        g_params_dome_action_delete_sequence,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        5948,  // help_offset
        144,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.test-sequence",
        "action",
        NULL,  // aliases
        g_params_dome_action_test_sequence,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        6093,  // help_offset
        171,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.pose-sequence",
        "action",
        NULL,  // aliases
        g_params_dome_action_pose_sequence,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        6265,  // help_offset
        319,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.get-sequence-last-run",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        6585,  // help_offset
        146,  // help_length
        g_fields_dome_api_get_sequence_last_run,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.api.get-sequence-pins",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        6732,  // help_offset
        160,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_BROWSER_ONLY,  // console_excluded
        NULL,  // output
        "Dashboard",  // console_page
    },
    {
        "dome.action.pin-sequence",
        "action",
        NULL,  // aliases
        g_params_dome_action_pin_sequence,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        6893,  // help_offset
        180,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_BROWSER_ONLY,  // console_excluded
        NULL,  // output
        "Dashboard",  // console_page
    },
    {
        "dome.api.get-take",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7074,  // help_offset
        185,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.action.arm-take",
        "action",
        NULL,  // aliases
        g_params_dome_action_arm_take,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7260,  // help_offset
        258,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_EDITOR_ONLY,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "dome.action.keep-take",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7519,  // help_offset
        157,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_EDITOR_ONLY,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "dome.api.get-take-file",
        "action",
        NULL,  // aliases
        g_params_dome_api_get_take_file,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7677,  // help_offset
        164,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "dome.action.restore-take-file",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7842,  // help_offset
        151,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Sequences",  // console_page
    },
    {
        "sound.action.play-track",
        "action",
        NULL,  // aliases
        g_params_sound_action_play_track,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        7994,  // help_offset
        103,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-scream",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8098,  // help_offset
        86,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-faint",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8185,  // help_offset
        99,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-leia",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8285,  // help_offset
        96,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-cantina-short",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8382,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-cantina-long",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8490,  // help_offset
        104,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-sw-theme",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8595,  // help_offset
        106,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-imperial-march",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8702,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-startup",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8813,  // help_offset
        101,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-disco",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        8915,  // help_offset
        114,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-track-happy",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9030,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.api.get-catalog",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9123,  // help_offset
        94,  // help_length
        g_fields_sound_api_get_catalog,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.api.refresh-catalog",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9218,  // help_offset
        123,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.api.play-banked",
        "action",
        NULL,  // aliases
        g_params_sound_api_play_banked,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9342,  // help_offset
        150,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.api.get-mood-map",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9493,  // help_offset
        109,  // help_length
        g_fields_sound_api_get_mood_map,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.set-mood-map",
        "action",
        NULL,  // aliases
        g_params_sound_action_set_mood_map,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9603,  // help_offset
        202,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.set-category-range",
        "action",
        NULL,  // aliases
        g_params_sound_action_set_category_range,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9806,  // help_offset
        172,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.query-status",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        9979,  // help_offset
        133,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.track-stop",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10113,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.quiet",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10206,  // help_offset
        106,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.set-volume",
        "action",
        NULL,  // aliases
        g_params_sound_action_set_volume,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10313,  // help_offset
        94,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.volume-up",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10408,  // help_offset
        77,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.volume-down",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10486,  // help_offset
        82,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.volume-preset-mid",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10569,  // help_offset
        93,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.volume-preset-max",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10663,  // help_offset
        89,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.volume-preset-min",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10753,  // help_offset
        90,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.dollar-command",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10844,  // help_offset
        105,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-on",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        10950,  // help_offset
        93,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-off",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11044,  // help_offset
        99,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-general",
        "action",
        g_aliases_sound_action_random_general,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11144,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-chatty",
        "action",
        g_aliases_sound_action_random_chatty,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11237,  // help_offset
        89,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-happy",
        "action",
        g_aliases_sound_action_random_happy,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11327,  // help_offset
        86,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-processing",
        "action",
        g_aliases_sound_action_random_processing,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11414,  // help_offset
        101,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-sad",
        "action",
        g_aliases_sound_action_random_sad,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11516,  // help_offset
        80,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-sentimental",
        "action",
        g_aliases_sound_action_random_sentimental,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11597,  // help_offset
        104,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-humming",
        "action",
        g_aliases_sound_action_random_humming,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11702,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-scream",
        "action",
        g_aliases_sound_action_random_scream,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11795,  // help_offset
        89,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-surprised",
        "action",
        g_aliases_sound_action_random_surprised,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11885,  // help_offset
        98,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-alert",
        "action",
        g_aliases_sound_action_random_alert,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        11984,  // help_offset
        86,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-snarky",
        "action",
        g_aliases_sound_action_random_snarky,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12071,  // help_offset
        89,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.random-whistle",
        "action",
        g_aliases_sound_action_random_whistle,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12161,  // help_offset
        92,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-next",
        "action",
        g_aliases_sound_action_play_next,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12254,  // help_offset
        96,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.action.play-previous",
        "action",
        g_aliases_sound_action_play_previous,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12351,  // help_offset
        105,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.status.current",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12457,  // help_offset
        121,  // help_length
        g_fields_sound_status_current,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.volume",
        "config",
        NULL,  // aliases
        g_params_sound_config_volume,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12579,  // help_offset
        98,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.random-min",
        "config",
        NULL,  // aliases
        g_params_sound_config_random_min,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12678,  // help_offset
        111,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.random-max",
        "config",
        NULL,  // aliases
        g_params_sound_config_random_max,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12790,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.mood-interval-quiet",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        12903,  // help_offset
        146,  // help_length
        NULL,  // fields
        true,  // is_query
        true,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.mood-interval-mid",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13050,  // help_offset
        147,  // help_length
        NULL,  // fields
        true,  // is_query
        true,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.mood-interval-full",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13198,  // help_offset
        150,  // help_length
        NULL,  // fields
        true,  // is_query
        true,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.mood-interval-awake-plus",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13349,  // help_offset
        148,  // help_length
        NULL,  // fields
        true,  // is_query
        true,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.startup-track",
        "config",
        NULL,  // aliases
        g_params_sound_config_startup_track,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13498,  // help_offset
        150,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.boot-complete-track",
        "config",
        NULL,  // aliases
        g_params_sound_config_boot_complete_track,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13649,  // help_offset
        160,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.network-down-track",
        "config",
        NULL,  // aliases
        g_params_sound_config_network_down_track,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13810,  // help_offset
        162,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.track-assignments",
        "config",
        NULL,  // aliases
        g_params_sound_config_track_assignments,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        13973,  // help_offset
        142,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.system-track-assignments",
        "config",
        NULL,  // aliases
        g_params_sound_config_system_track_assignments,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14116,  // help_offset
        168,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.category-ranges",
        "config",
        NULL,  // aliases
        g_params_sound_config_category_ranges,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14285,  // help_offset
        172,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "sound.config.mood-category-map",
        "config",
        NULL,  // aliases
        g_params_sound_config_mood_category_map,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14458,  // help_offset
        215,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.open",
        "action",
        NULL,  // aliases
        g_params_servo_action_open,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14674,  // help_offset
        90,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.close",
        "action",
        NULL,  // aliases
        g_params_servo_action_close,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14765,  // help_offset
        94,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.set-position",
        "action",
        NULL,  // aliases
        g_params_servo_action_set_position,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14860,  // help_offset
        138,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.nudge",
        "action",
        NULL,  // aliases
        g_params_servo_action_nudge,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        14999,  // help_offset
        185,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.travel",
        "action",
        NULL,  // aliases
        g_params_servo_action_travel,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        15185,  // help_offset
        170,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.hold",
        "action",
        NULL,  // aliases
        g_params_servo_action_hold,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        15356,  // help_offset
        205,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.release",
        "action",
        NULL,  // aliases
        g_params_servo_action_release,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        15562,  // help_offset
        142,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.centre-all",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        15705,  // help_offset
        195,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.config.cadence-floor",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        15901,  // help_offset
        237,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.stop",
        "action",
        NULL,  // aliases
        g_params_servo_action_stop,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16139,  // help_offset
        109,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.action.toggle-arm1",
        "action",
        g_aliases_servo_action_toggle_arm1,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16249,  // help_offset
        91,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "arm1",  // output
        NULL,  // console_page
    },
    {
        "servo.action.toggle-arm2",
        "action",
        g_aliases_servo_action_toggle_arm2,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16341,  // help_offset
        91,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "arm2",  // output
        NULL,  // console_page
    },
    {
        "servo.action.toggle-aux1",
        "action",
        g_aliases_servo_action_toggle_aux1,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16433,  // help_offset
        91,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux1",  // output
        NULL,  // console_page
    },
    {
        "servo.action.toggle-aux2",
        "action",
        g_aliases_servo_action_toggle_aux2,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16525,  // help_offset
        91,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux2",  // output
        NULL,  // console_page
    },
    {
        "servo.action.toggle-aux3",
        "action",
        g_aliases_servo_action_toggle_aux3,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16617,  // help_offset
        91,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux3",  // output
        NULL,  // console_page
    },
    {
        "servo.action.puppet-part",
        "action",
        g_aliases_servo_action_puppet_part,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16709,  // help_offset
        130,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.status.current",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16840,  // help_offset
        88,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "servo.api.get-outputs",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        16929,  // help_offset
        161,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "aux.action.led-color",
        "action",
        NULL,  // aliases
        g_params_aux_action_led_color,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17091,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "aux.action.led-effect",
        "action",
        NULL,  // aliases
        g_params_aux_action_led_effect,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17199,  // help_offset
        96,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "aux.status.led-state",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17296,  // help_offset
        117,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "aux.config.led-count",
        "config",
        NULL,  // aliases
        g_params_aux_config_led_count,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17414,  // help_offset
        153,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.set-mode",
        "action",
        g_aliases_system_action_set_mode,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17568,  // help_offset
        121,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.estop",
        "action",
        g_aliases_system_action_estop,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        true,  // safety_critical
        17690,  // help_offset
        90,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.estop-clear",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        true,  // safety_critical
        17781,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.enable-web-control",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        17892,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.disable-web-control",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18000,  // help_offset
        125,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.reboot",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18126,  // help_offset
        78,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.reboot-wifi-module",
        "action",
        NULL,  // aliases
        NULL,
        PA_CAP_HOSTED_WIFI,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18205,  // help_offset
        233,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.set-mood",
        "action",
        NULL,  // aliases
        g_params_system_action_set_mood,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18439,  // help_offset
        127,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.sleep",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        18567,  // help_offset
        134,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.wake",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        18702,  // help_offset
        104,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.sleep-toggle",
        "action",
        g_aliases_system_action_sleep_toggle,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18807,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.event.drives-engaged",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18918,  // help_offset
        79,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.event.dome-enabled",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        18998,  // help_offset
        68,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.event.boot-complete",
        "event",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19067,  // help_offset
        107,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.sleep-mode",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19175,  // help_offset
        106,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.mood",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19282,  // help_offset
        104,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.mood",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19387,  // help_offset
        71,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_arm1",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19459,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "arm1",  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_arm2",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19592,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "arm2",  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_aux1",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19725,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux1",  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_aux2",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19858,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux2",  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_aux3",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        19991,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        "aux3",  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_dome_esc",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20124,  // help_offset
        160,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch1",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20285,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch2",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20383,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch3",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20481,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch4",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20579,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch5",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20677,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_rc_ch6",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20775,  // help_offset
        97,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_drive",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20873,  // help_offset
        93,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_audio",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        20967,  // help_offset
        67,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.enable_protor2link",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21035,  // help_offset
        83,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.config.log-level",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21119,  // help_offset
        106,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.health",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21226,  // help_offset
        130,  // help_length
        g_fields_system_status_health,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.dashboard-health",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21357,  // help_offset
        130,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.logs",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21488,  // help_offset
        56,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.wifi",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21545,  // help_offset
        88,  // help_length
        g_fields_system_status_wifi,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.status.hosted-link",
        "status",
        NULL,  // aliases
        NULL,
        PA_CAP_HOSTED_WIFI,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21634,  // help_offset
        162,  // help_length
        g_fields_system_status_hosted_link,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "wifi.config.settings",
        "config",
        NULL,  // aliases
        g_params_wifi_config_settings,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        21797,  // help_offset
        281,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.status.serial-link",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22079,  // help_offset
        129,  // help_length
        g_fields_dome_status_serial_link,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-identity",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22209,  // help_offset
        127,  // help_length
        g_fields_system_api_get_identity,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-components",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22337,  // help_offset
        158,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.set-identity",
        "action",
        NULL,  // aliases
        g_params_system_action_set_identity,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22496,  // help_offset
        216,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-profiler",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        PA_HEAP_PROFILE,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22713,  // help_offset
        118,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.profiler-trace-start",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        PA_HEAP_TRACING,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22832,  // help_offset
        158,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.profiler-trace-stop",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        PA_HEAP_TRACING,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        22991,  // help_offset
        120,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-coredump-status",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23112,  // help_offset
        102,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-coredump",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23215,  // help_offset
        79,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Maintenance",  // console_page
    },
    {
        "system.action.erase-coredump",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23295,  // help_offset
        99,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-admission-trace",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        PA_ADMISSION_TRACE,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23395,  // help_offset
        156,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.api.get-validation",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23552,  // help_offset
        110,  // help_length
        g_fields_system_api_get_validation,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.action.upload-firmware",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23663,  // help_offset
        101,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Firmware",  // console_page
    },
    {
        "system.action.upload-filesystem",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23765,  // help_offset
        118,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "Firmware",  // console_page
    },
    {
        "system.api.event-stream",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23884,  // help_offset
        79,  // help_length
        NULL,  // fields
        false,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "rc.status.snapshot",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        23964,  // help_offset
        109,  // help_length
        g_fields_rc_status_snapshot,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "rc.action.toggle-debug",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24074,  // help_offset
        90,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "rc.api.get-bindable-actions",
        "status",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24165,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "rc.api.get-map",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24276,  // help_offset
        69,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "RC Control",  // console_page
    },
    {
        "rc.action.set-map",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24346,  // help_offset
        102,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_FILE_TRANSFER,  // console_excluded
        NULL,  // output
        "RC Control",  // console_page
    },
    {
        "rc.action.test-bindable",
        "action",
        NULL,  // aliases
        g_params_rc_action_test_bindable,
        1,  // available_on_board
        1,  // available_in_build
        true,  // requires_web_control
        false,  // safety_critical
        24449,  // help_offset
        141,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "rc.config.mode",
        "config",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24591,  // help_offset
        171,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.vader",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24763,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.hello",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24876,  // help_offset
        110,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.nod",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        24987,  // help_offset
        117,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.flutter",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25105,  // help_offset
        122,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.bloom",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25228,  // help_offset
        118,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.leia",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25347,  // help_offset
        128,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.alarm",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25476,  // help_offset
        112,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.heart",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25589,  // help_offset
        117,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.reset",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25707,  // help_offset
        134,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.pies",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25842,  // help_offset
        127,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.low",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        25970,  // help_offset
        132,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.openall",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26103,  // help_offset
        149,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.cantina",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26253,  // help_offset
        140,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.rockmarch",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26394,  // help_offset
        134,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.scream",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26529,  // help_offset
        114,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "dome.seq.overload",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26644,  // help_offset
        123,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_NONE,  // console_excluded
        NULL,  // output
        NULL,  // console_page
    },
    {
        "system.console",
        "action",
        NULL,  // aliases
        NULL,
        1,  // available_on_board
        1,  // available_in_build
        false,  // requires_web_control
        false,  // safety_critical
        26768,  // help_offset
        133,  // help_length
        NULL,  // fields
        true,  // is_query
        false,  // read_only
        CONSOLE_EXCLUSION_CONSOLE_ITSELF,  // console_excluded
        NULL,  // output
        "Dashboard",  // console_page
    },
};

static const size_t g_catalogCount = sizeof(g_catalogEntries) / sizeof(g_catalogEntries[0]);

// =============================================================================
// Public API
// =============================================================================

const ConsoleCatalogEntry* consoleCatalogGetEntries(size_t* out_count) {
    if (out_count) {
        *out_count = g_catalogCount;
    }
    return g_catalogEntries;
}

const ConsoleCatalogEntry* consoleCatalogFindByName(const char* name) {
    if (!name) return NULL;
    for (size_t i = 0; i < g_catalogCount; ++i) {
        if (strcmp(g_catalogEntries[i].name, name) == 0) {
            return &g_catalogEntries[i];
        }
    }
    return NULL;
}

size_t consoleCatalogGetCount(void) {
    return g_catalogCount;
}

// =============================================================================
// Named Body/Dome Sequences (registry marcduino_cmd, literal DM:<NAME> only)
// =============================================================================

typedef struct {
    const char* operationName;
    const char* sequence;
} ConsoleSequenceRow;

static const ConsoleSequenceRow g_sequenceRows[] = {
    {"dome.seq.vader", "DM:VADER"},
    {"dome.seq.hello", "DM:HELLO"},
    {"dome.seq.nod", "DM:NOD"},
    {"dome.seq.flutter", "DM:FLUTTER"},
    {"dome.seq.bloom", "DM:BLOOM"},
    {"dome.seq.leia", "DM:LEIA"},
    {"dome.seq.alarm", "DM:ALARM"},
    {"dome.seq.heart", "DM:HEART"},
    {"dome.seq.reset", "DM:RESET"},
    {"dome.seq.pies", "DM:PIES"},
    {"dome.seq.low", "DM:LOW"},
    {"dome.seq.openall", "DM:OPENALL"},
    {"dome.seq.cantina", "DM:CANTINA"},
    {"dome.seq.rockmarch", "DM:ROCKMARCH"},
    {"dome.seq.scream", "DM:SCREAM"},
    {"dome.seq.overload", "DM:OVERLOAD"},
};

static const size_t g_sequenceRowCount = sizeof(g_sequenceRows) / sizeof(g_sequenceRows[0]);

const char* consoleCatalogSequenceFor(const char* operationName) {
    if (!operationName) return NULL;
    for (size_t i = 0; i < g_sequenceRowCount; ++i) {
        if (strcmp(g_sequenceRows[i].operationName, operationName) == 0) {
            return g_sequenceRows[i].sequence;
        }
    }
    return NULL;
}

// =============================================================================
// Why an Operation Is Never on the Console (registry console: excluded:)
// =============================================================================

static const char* const g_exclusionNames[] = { "file-transfer", "editor-only", "browser-only", "console-itself" };

const char* consoleCatalogExclusionName(uint8_t exclusion) {
    const size_t count = sizeof(g_exclusionNames) / sizeof(g_exclusionNames[0]);
    if (exclusion == CONSOLE_EXCLUSION_NONE || exclusion > count) return NULL;
    return g_exclusionNames[exclusion - 1];
}
