#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Rpy/RpyInterpreterTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMyProjectUnitTest, "MyProject.UnitTest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool RunTests(const TArray<FString>& Args)
{
    bool bSuccess = true;

    // Initialize the Unreal Test Framework
    FAutomationTestFramework& TestFramework = FAutomationTestFramework::Get();
    TestFramework.Initialize(&Args);

    // Register your test module
    TestFramework.RegisterAutomationTestModule(RpyInterpreterTest::Get());

    // Run the tests
    bSuccess &= TestFramework.RunAutomationTests(TEXT("MyProject"));

    // Shutdown the test framework
    TestFramework.Shutdown();

    return bSuccess;
}

int main(int argc, char* argv[])
{
    // Convert command-line arguments to FString array
    TArray<FString> Args;
    for (int i = 0; i < argc; i++)
    {
        Args.Add(UTF8_TO_TCHAR(argv[i]));
    }

    // Run the tests
    bool bSuccess = RunTests(Args);

    // Return success or failure
    return bSuccess ? 0 : 1;
}
