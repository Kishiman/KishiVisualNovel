#include "EditorScriptExecutionTest.h"

class RpyInterpreterTest : public FEditorScriptExecutionTest
{
    GENERATED_BODY()
};
void RpyInterpreterTest::BeginTest(const FString& TestName)
{
    // Code to run before the test
}

void RpyInterpreterTest::EndTest(const FString& TestName, const bool bSucceeded)
{
    // Code to run after the test
}

void RpyInterpreterTest::TestBody()
{
    int a = 2;
    int b = 3;
    int result = a + b;
    TestEqual(TEXT("Addition Test"), result, 5);
}

