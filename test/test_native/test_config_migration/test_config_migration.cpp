// =============================================================================
// test/test_native/test_config_migration/test_config_migration.cpp
//
// Native tests for config schema migrations.
// Verifies that old NVS keys are properly migrated to new keys and deleted.
// =============================================================================

#include <unity.h>
#include <Preferences.h>

#include "config_store.h"
#include "dome_bearing.h"  // DomeTurnDirection - the stored turn word

// Helper to simulate old schema 2 data by directly writing to NVS
static void setupSchema2Data(Preferences& prefs) {
    // Set schema version to 2 (old)
    prefs.putUChar("schema_ver", 2);

    // Write old component toggle keys (schema 2)
    prefs.putBool("en_s1", true);    // will become en_drive
    prefs.putBool("en_dome", true);  // en_dome_esc at schema 3, en_dome_rot at schema 4
    prefs.putBool("en_s3", true);    // will become en_r2link
    prefs.putBool("en_s2", true);    // will become en_audio

    // Write old RC audio binding keys (schema 2)
    prefs.putString("rcp_snd", "ch:1");  // will become rcp_aud
    prefs.putString("rcs_snd", "ch:2");  // will become rcs_aud
    prefs.putString("rc_sound", "sw:0:1"); // will become rc_aud
}

void setUp(void) {
    // Initialize preferences for each test
}

void tearDown(void) {
    // Clean up after each test
}

void test_schema2_to_3_migration_renames_component_toggles(void) {
    // Test: verify old component toggle keys are migrated to new keys
    Preferences prefs;
    prefs.begin("test_config1", false);
    prefs.clear();

    setupSchema2Data(prefs);

    // Verify old keys exist before migration
    TEST_ASSERT_TRUE(prefs.isKey("en_s1"));
    TEST_ASSERT_TRUE(prefs.isKey("en_dome"));
    TEST_ASSERT_TRUE(prefs.isKey("en_s3"));
    TEST_ASSERT_TRUE(prefs.isKey("en_s2"));

    // Load config, which triggers migration
    ConfigSnapshot snap = {};
    configLoad(prefs, &snap);

    // Verify new keys now hold the migrated values
    TEST_ASSERT_TRUE(prefs.getBool("en_drive", false));
    TEST_ASSERT_TRUE(prefs.getBool("en_dome_rot", false));
    TEST_ASSERT_TRUE(prefs.getBool("en_r2link", false));
    TEST_ASSERT_TRUE(prefs.getBool("en_audio", false));

    // Verify old keys have been deleted
    TEST_ASSERT_FALSE(prefs.isKey("en_s1"));
    TEST_ASSERT_FALSE(prefs.isKey("en_dome"));
    TEST_ASSERT_FALSE(prefs.isKey("en_dome_esc"));
    TEST_ASSERT_FALSE(prefs.isKey("en_s3"));
    TEST_ASSERT_FALSE(prefs.isKey("en_s2"));

    // Verify snapshot holds the migrated values
    TEST_ASSERT_TRUE(snap.system.enable_drive);
    TEST_ASSERT_TRUE(snap.system.enable_dome_esc);
    TEST_ASSERT_TRUE(snap.system.enable_protor2link);
    TEST_ASSERT_TRUE(snap.system.enable_audio);

    prefs.end();
}

void test_schema2_to_3_migration_renames_rc_audio_bindings(void) {
    // Test: verify old RC audio binding keys are migrated to new keys
    Preferences prefs;
    prefs.begin("test_config2", false);
    prefs.clear();

    setupSchema2Data(prefs);

    // Verify old RC audio keys exist before migration
    TEST_ASSERT_TRUE(prefs.isKey("rcp_snd"));
    TEST_ASSERT_TRUE(prefs.isKey("rcs_snd"));
    TEST_ASSERT_TRUE(prefs.isKey("rc_sound"));

    // Load config, which triggers migration
    ConfigSnapshot snap = {};
    configLoad(prefs, &snap);

    // Verify new keys now hold the migrated string values
    String rcp_aud = prefs.getString("rcp_aud", "");
    String rcs_aud = prefs.getString("rcs_aud", "");
    String rc_aud = prefs.getString("rc_aud", "");

    TEST_ASSERT_EQUAL_STRING("ch:1", rcp_aud.c_str());
    TEST_ASSERT_EQUAL_STRING("ch:2", rcs_aud.c_str());
    TEST_ASSERT_EQUAL_STRING("sw:0:1", rc_aud.c_str());

    // Verify old keys have been deleted
    TEST_ASSERT_FALSE(prefs.isKey("rcp_snd"));
    TEST_ASSERT_FALSE(prefs.isKey("rcs_snd"));
    TEST_ASSERT_FALSE(prefs.isKey("rc_sound"));

    prefs.end();
}

void test_schema2_to_3_migration_does_not_repeat(void) {
    // Test: verify migration only runs once (schema version is updated)
    Preferences prefs;
    prefs.begin("test_config3", false);
    prefs.clear();

    setupSchema2Data(prefs);

    // First load: migration happens
    ConfigSnapshot snap1 = {};
    configLoad(prefs, &snap1);

    // Verify schema version is now current
    uint8_t newVersion = prefs.getUChar("schema_ver", 0);
    TEST_ASSERT_EQUAL_INT(CONFIG_SCHEMA_VERSION, newVersion);

    // Second load: migration should NOT run again
    // If it did, it would try to read old keys that no longer exist
    ConfigSnapshot snap2 = {};
    bool ok = configLoad(prefs, &snap2);
    TEST_ASSERT_TRUE(ok);

    // Verify migrated values are still correct
    TEST_ASSERT_TRUE(snap2.system.enable_drive);
    TEST_ASSERT_TRUE(snap2.system.enable_dome_esc);
    TEST_ASSERT_TRUE(snap2.system.enable_protor2link);
    TEST_ASSERT_TRUE(snap2.system.enable_audio);

    prefs.end();
}

void test_schema2_to_3_migration_handles_missing_old_keys(void) {
    // Test: verify migration gracefully handles missing old keys (defaults used)
    Preferences prefs;
    prefs.begin("test_config4", false);
    prefs.clear();

    // Set schema version to 2 but don't set any old keys (they're missing)
    prefs.putUChar("schema_ver", 2);

    // Load config, which triggers migration
    ConfigSnapshot snap = {};
    configLoad(prefs, &snap);

    // Verify snapshot holds default values (false for toggles)
    TEST_ASSERT_FALSE(snap.system.enable_drive);
    TEST_ASSERT_FALSE(snap.system.enable_dome_esc);
    TEST_ASSERT_FALSE(snap.system.enable_protor2link);
    TEST_ASSERT_FALSE(snap.system.enable_audio);

    // Verify schema version is now current
    uint8_t newVersion = prefs.getUChar("schema_ver", 0);
    TEST_ASSERT_EQUAL_INT(CONFIG_SCHEMA_VERSION, newVersion);

    prefs.end();
}

// #484: the boot load opened the store read-only, so on a device every
// migration write and the schema stamp were dropped. These load through
// configOpenForLoad(), the boot's own open, and read the store back through a
// handle of their own.
static void seedConfigNamespace(uint8_t schema, uint8_t logLevel) {
    Preferences seed;
    seed.begin(NVS_NAMESPACE, false);
    seed.clear();
    setupSchema2Data(seed);
    seed.putUChar("schema_ver", schema);
    seed.putUChar("log_level", logLevel);
    seed.end();
}

void test_boot_open_lands_schema2_migration_and_stamp(void) {
    seedConfigNamespace(2, 3);

    Preferences prefs;
    TEST_ASSERT_TRUE(configOpenForLoad(prefs));
    ConfigSnapshot snap = {};
    configLoad(prefs, &snap);
    prefs.end();

    Preferences check;
    check.begin(NVS_NAMESPACE, true);
    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION, check.getUChar("schema_ver", 0));
    TEST_ASSERT_TRUE(check.getBool("en_drive", false));
    TEST_ASSERT_TRUE(check.getBool("en_audio", false));
    TEST_ASSERT_EQUAL_STRING("sw:0:1", check.getString("rc_aud", "").c_str());
    TEST_ASSERT_FALSE(check.isKey("en_s1"));
    TEST_ASSERT_FALSE(check.isKey("rc_sound"));
    // Schema 2 is past the log level renumber: stored as it was.
    TEST_ASSERT_EQUAL_UINT8(3, check.getUChar("log_level", 0));
    check.end();
}

void test_boot_open_lands_log_level_renumber_once(void) {
    // Schema 1's Info (2) is Warn's slot now: renumbered to 3, stored once.
    seedConfigNamespace(1, 2);

    for (int boot = 0; boot < 2; ++boot) {
        Preferences prefs;
        TEST_ASSERT_TRUE(configOpenForLoad(prefs));
        ConfigSnapshot snap = {};
        configLoad(prefs, &snap);
        prefs.end();
        TEST_ASSERT_EQUAL_UINT8(3, snap.system.logLevel);
    }

    Preferences check;
    check.begin(NVS_NAMESPACE, true);
    TEST_ASSERT_EQUAL_UINT8(3, check.getUChar("log_level", 0));
    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION, check.getUChar("schema_ver", 0));
    TEST_ASSERT_FALSE(check.isKey(CONFIG_LOG_LEVEL_SCHEMA1_KEY));
    check.end();
}

void test_renumber_cut_off_before_the_stamp_is_not_renumbered_twice(void) {
    // What a boot cut off between the log level write and the stamp leaves:
    // schema 1, the renumbered Info (3), and the schema-1 Info (2) kept.
    seedConfigNamespace(1, 3);
    {
        Preferences seed;
        seed.begin(NVS_NAMESPACE, false);
        seed.putUChar(CONFIG_LOG_LEVEL_SCHEMA1_KEY, 2);
        seed.end();
    }

    Preferences prefs;
    TEST_ASSERT_TRUE(configOpenForLoad(prefs));
    ConfigSnapshot snap = {};
    configLoad(prefs, &snap);
    prefs.end();
    TEST_ASSERT_EQUAL_UINT8(3, snap.system.logLevel);

    Preferences check;
    check.begin(NVS_NAMESPACE, true);
    TEST_ASSERT_EQUAL_UINT8(3, check.getUChar("log_level", 0));
    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION, check.getUChar("schema_ver", 0));
    TEST_ASSERT_FALSE(check.isKey(CONFIG_LOG_LEVEL_SCHEMA1_KEY));
    check.end();
}

void test_future_schema_loads_defaults_and_keeps_the_store(void) {
    // A rollback from a newer image: defaults in RAM, and the newer store
    // left as it was, schema included, so no later boot reads it as current.
    seedConfigNamespace(CONFIG_SCHEMA_VERSION + 1, 4);
    {
        Preferences seed;
        seed.begin(NVS_NAMESPACE, false);
        seed.putBool("en_drive", true);
        seed.end();
    }

    for (int boot = 0; boot < 2; ++boot) {
        Preferences prefs;
        TEST_ASSERT_TRUE(configOpenForLoad(prefs));
        ConfigSnapshot snap = {};
        TEST_ASSERT_FALSE(configLoad(prefs, &snap));
        prefs.end();
        TEST_ASSERT_FALSE(snap.system.enable_drive);
    }

    Preferences check;
    check.begin(NVS_NAMESPACE, true);
    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION + 1, check.getUChar("schema_ver", 0));
    TEST_ASSERT_TRUE(check.getBool("en_drive", false));
    check.end();
}

void test_failed_rename_leaves_old_key_and_no_stamp(void) {
    // A rename that does not land keeps its old key and the old schema, so
    // the next boot migrates again instead of loading the toggle as a default.
    Preferences prefs;
    prefs.begin("test_config5", false);
    prefs.clear();
    setupSchema2Data(prefs);
    prefs.failNextIntegerWrites(1);  // the first putBool: en_s1 -> en_drive

    ConfigSnapshot first = {};
    configLoad(prefs, &first);
    TEST_ASSERT_EQUAL_UINT8(2, prefs.getUChar("schema_ver", 0));
    TEST_ASSERT_TRUE(prefs.isKey("en_s1"));

    ConfigSnapshot second = {};
    configLoad(prefs, &second);
    TEST_ASSERT_TRUE(second.system.enable_drive);
    TEST_ASSERT_FALSE(prefs.isKey("en_s1"));
    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION, prefs.getUChar("schema_ver", 0));
    prefs.end();
}

// #504: a schema-3 store comes out with every Dome Rotation Setting under its
// new key, in its own type, and no old key left; the two unread speeds go.
void test_schema3_to_4_migration_carries_dome_rotation_keys(void) {
    Preferences prefs;
    prefs.begin("test_config6", false);
    prefs.clear();
    prefs.putUChar("schema_ver", 3);
    prefs.putBool("en_dome_esc", true);
    prefs.putUShort("dome_neu", 1510);
    prefs.putUShort("dome_minp", 1100);
    prefs.putUShort("dome_maxp", 1900);
    prefs.putUChar("dome_pct", 40);
    prefs.putBool("dome_rnd_en", true);
    prefs.putUChar("dome_rnd_spd", 25);
    prefs.putUChar("dome_rnd_pmin", 3);
    prefs.putUChar("dome_rnd_pmax", 9);
    prefs.putUShort("dome_rnd_ms", 1800);
    prefs.putUShort("dome_turn_ms", 7200);
    prefs.putUChar("dome_turn_pct", 60);
    prefs.putUChar("dome_turn_dir", (uint8_t)DOME_TURN_DIR_CCW);
    prefs.putULong("dome_min", 0);
    prefs.putULong("dome_max", 0);
    prefs.putString("dome_wip", "192.168.4.2");

    ConfigSnapshot snap = {};
    TEST_ASSERT_TRUE(configLoad(prefs, &snap));

    TEST_ASSERT_EQUAL_UINT8(CONFIG_SCHEMA_VERSION, prefs.getUChar("schema_ver", 0));
    TEST_ASSERT_TRUE(snap.system.enable_dome_esc);
    TEST_ASSERT_EQUAL_UINT16(1510, snap.dome.dome_neutral_us);
    TEST_ASSERT_EQUAL_UINT16(1100, snap.dome.dome_min_pulse_us);
    TEST_ASSERT_EQUAL_UINT16(1900, snap.dome.dome_max_pulse_us);
    TEST_ASSERT_EQUAL_UINT8(40, snap.dome.dome_speed_limit_pct);
    TEST_ASSERT_TRUE(snap.dome.dome_rnd_enable);
    TEST_ASSERT_EQUAL_UINT8(25, snap.dome.dome_rnd_speed_pct);
    TEST_ASSERT_EQUAL_UINT8(3, snap.dome.dome_rnd_pause_min);
    TEST_ASSERT_EQUAL_UINT8(9, snap.dome.dome_rnd_pause_max);
    TEST_ASSERT_EQUAL_UINT16(1800, snap.dome.dome_rnd_move_ms);
    TEST_ASSERT_EQUAL_UINT16(7200, snap.dome.dome_full_turn_ms);
    TEST_ASSERT_EQUAL_UINT8(60, snap.dome.dome_full_turn_pct);
    TEST_ASSERT_EQUAL_UINT8(DOME_TURN_DIR_CCW, snap.dome.dome_positive_turn);
    TEST_ASSERT_EQUAL_STRING("192.168.4.2", snap.dome.dome_wifi_peer_ip);

    static const char* const kNewKeys[] = {
        "en_dome_rot",   "drot_neu",      "drot_minp",   "drot_maxp",    "drot_pct",
        "drot_rnd_en",   "drot_rnd_spd",  "drot_rnd_pmin", "drot_rnd_pmax", "drot_rnd_ms",
        "drot_turn_ms",  "drot_turn_pct", "drot_turn_dir",
    };
    static const char* const kGoneKeys[] = {
        "en_dome_esc",   "dome_neu",      "dome_minp",   "dome_maxp",    "dome_pct",
        "dome_rnd_en",   "dome_rnd_spd",  "dome_rnd_pmin", "dome_rnd_pmax", "dome_rnd_ms",
        "dome_turn_ms",  "dome_turn_pct", "dome_turn_dir", "dome_min",   "dome_max",
    };
    for (const char* key : kNewKeys) {
        TEST_ASSERT_TRUE_MESSAGE(prefs.isKey(key), key);
    }
    for (const char* key : kGoneKeys) {
        TEST_ASSERT_FALSE_MESSAGE(prefs.isKey(key), key);
    }
    TEST_ASSERT_TRUE(prefs.isKey("dome_wip"));
    prefs.end();
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_boot_open_lands_schema2_migration_and_stamp);
    RUN_TEST(test_boot_open_lands_log_level_renumber_once);
    RUN_TEST(test_failed_rename_leaves_old_key_and_no_stamp);
    RUN_TEST(test_renumber_cut_off_before_the_stamp_is_not_renumbered_twice);
    RUN_TEST(test_future_schema_loads_defaults_and_keeps_the_store);
    RUN_TEST(test_schema2_to_3_migration_renames_component_toggles);
    RUN_TEST(test_schema2_to_3_migration_renames_rc_audio_bindings);
    RUN_TEST(test_schema2_to_3_migration_does_not_repeat);
    RUN_TEST(test_schema2_to_3_migration_handles_missing_old_keys);
    RUN_TEST(test_schema3_to_4_migration_carries_dome_rotation_keys);
    return UNITY_END();
}
