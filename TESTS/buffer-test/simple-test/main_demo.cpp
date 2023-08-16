#include "mbed.h"
#include "greentea-client/test_env.h"
#include "utest/utest.h"
#include "unity/unity.h"

using namespace utest::v1;

static void test_simple() {
    TEST_ASSERT_EQUAL(0, 0);
    printf("Simple test called\n");
}

status_t test_repeats_setup(const Case *const source, const size_t index_of_case) {
    // Call the default handler for proper reporting
    status_t status = greentea_case_setup_handler(source, index_of_case);
    printf("Setting up for '%s' (status %d)\n", source->get_description(), status);
    return status;
}

control_t test_repeats(const size_t call_count) {
    printf("Called for the %u. time\n", call_count);
    TEST_ASSERT_NOT_EQUAL(3, call_count);
    // Specify how often this test is repeated ie. n total calls
    return (call_count < 2) ? CaseRepeatAll : CaseNext;
}

void test_callback_validate() {
    // You may also use assertions here!
    TEST_ASSERT_EQUAL_PTR(0, 0);
    // Validate the callback
    Harness::validate_callback();
}

control_t test_asynchronous() {
    TEST_ASSERT_TRUE_MESSAGE(true, "(true == false) o_O");
    // Set up a callback in the future. This may also be an interrupt!
    EventQueue *queue = mbed_event_queue();
    queue->call_in(100, test_callback_validate);
    // Set a 200ms timeout starting from now
    return CaseTimeout(200);
}

control_t test_asynchronous_timeout(const size_t call_count) {
    TEST_ASSERT_TRUE_MESSAGE(true, "(true == false) o_O");
    // Set a 200ms timeout starting from now,
    // but automatically repeat only this handler on timeout.
    if (call_count >= 5) {
        // but after the 5th call, the callback finally gets validated
        EventQueue *queue = mbed_event_queue();
        queue->call_in(100, test_callback_validate);
    }
    return CaseRepeatHandlerOnTimeout(200);
}

// Custom setup handler required for proper Greentea support
status_t greentea_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(20, "default_auto");
    // Call the default reporting function
    return greentea_test_setup_handler(number_of_cases);
}

// Specify all your test cases here
Case cases[] = {
    Case("Simple Test", test_simple),
    Case("Repeating Test", test_repeats_setup, test_repeats),
    Case("Asynchronous Test (200ms timeout)", test_asynchronous),
    Case("Asynchronous Timeout Repeat", test_asynchronous_timeout)
};

// Declare your test specification with a custom setup handler
Specification specification(greentea_setup, cases);

int main_d()
{   // Run the test specification
    Harness::run(specification);
}