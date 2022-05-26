#include <CUnit/Basic.h>
#include "UnitTest.h"


int main(void) {


    /* Initialise registry */
    CU_ErrorCode initializeRegistryStatus = CU_initialize_registry();

    if (initializeRegistryStatus != CUE_SUCCESS)
        return CU_get_error();


    /* Initialise Suite */
    CU_pSuite suite;

    suite = CU_add_suite("Test-Suite", nullptr, nullptr);
    if (suite == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add tests */
    CU_pTest addTestStatus;

    addTestStatus = CU_add_test(suite, "Test-Frechet", UnitTest::testFrechet);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-String", UnitTest::testString);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Curve", UnitTest::testCurve);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Vector", UnitTest::testVector);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Random", UnitTest::testRandom);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Arithmetic", UnitTest::testArithmetic);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Grid", UnitTest::testGrid);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    addTestStatus = CU_add_test(suite, "Test-Discrete-Snapping", UnitTest::testDiscreteSnapping);
    if (addTestStatus == nullptr) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run tests */
    CU_ErrorCode runTestsStatus = CU_basic_run_tests();
    if (runTestsStatus != CUE_SUCCESS) {
        CU_cleanup_registry();
        return CU_get_error();
    }


    /* Clean up registry */
    CU_cleanup_registry();

}