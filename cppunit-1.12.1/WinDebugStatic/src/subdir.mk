################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AdditionalMessage.cpp \
../src/Asserter.cpp \
../src/BeOsDynamicLibraryManager.cpp \
../src/BriefTestProgressListener.cpp \
../src/CompilerOutputter.cpp \
../src/DefaultProtector.cpp \
../src/DllMain.cpp \
../src/DynamicLibraryManager.cpp \
../src/DynamicLibraryManagerException.cpp \
../src/Exception.cpp \
../src/Message.cpp \
../src/PlugInManager.cpp \
../src/PlugInParameters.cpp \
../src/Protector.cpp \
../src/ProtectorChain.cpp \
../src/RepeatedTest.cpp \
../src/ShlDynamicLibraryManager.cpp \
../src/SourceLine.cpp \
../src/StringTools.cpp \
../src/SynchronizedObject.cpp \
../src/Test.cpp \
../src/TestAssert.cpp \
../src/TestCase.cpp \
../src/TestCaseDecorator.cpp \
../src/TestComposite.cpp \
../src/TestDecorator.cpp \
../src/TestFactoryRegistry.cpp \
../src/TestFailure.cpp \
../src/TestLeaf.cpp \
../src/TestNamer.cpp \
../src/TestPath.cpp \
../src/TestPlugInDefaultImpl.cpp \
../src/TestResult.cpp \
../src/TestResultCollector.cpp \
../src/TestRunner.cpp \
../src/TestSetUp.cpp \
../src/TestSuccessListener.cpp \
../src/TestSuite.cpp \
../src/TestSuiteBuilderContext.cpp \
../src/TextOutputter.cpp \
../src/TextTestProgressListener.cpp \
../src/TextTestResult.cpp \
../src/TextTestRunner.cpp \
../src/TypeInfoHelper.cpp \
../src/UnixDynamicLibraryManager.cpp \
../src/Win32DynamicLibraryManager.cpp \
../src/XmlDocument.cpp \
../src/XmlElement.cpp \
../src/XmlOutputter.cpp \
../src/XmlOutputterHook.cpp 

OBJS += \
./src/AdditionalMessage.o \
./src/Asserter.o \
./src/BeOsDynamicLibraryManager.o \
./src/BriefTestProgressListener.o \
./src/CompilerOutputter.o \
./src/DefaultProtector.o \
./src/DllMain.o \
./src/DynamicLibraryManager.o \
./src/DynamicLibraryManagerException.o \
./src/Exception.o \
./src/Message.o \
./src/PlugInManager.o \
./src/PlugInParameters.o \
./src/Protector.o \
./src/ProtectorChain.o \
./src/RepeatedTest.o \
./src/ShlDynamicLibraryManager.o \
./src/SourceLine.o \
./src/StringTools.o \
./src/SynchronizedObject.o \
./src/Test.o \
./src/TestAssert.o \
./src/TestCase.o \
./src/TestCaseDecorator.o \
./src/TestComposite.o \
./src/TestDecorator.o \
./src/TestFactoryRegistry.o \
./src/TestFailure.o \
./src/TestLeaf.o \
./src/TestNamer.o \
./src/TestPath.o \
./src/TestPlugInDefaultImpl.o \
./src/TestResult.o \
./src/TestResultCollector.o \
./src/TestRunner.o \
./src/TestSetUp.o \
./src/TestSuccessListener.o \
./src/TestSuite.o \
./src/TestSuiteBuilderContext.o \
./src/TextOutputter.o \
./src/TextTestProgressListener.o \
./src/TextTestResult.o \
./src/TextTestRunner.o \
./src/TypeInfoHelper.o \
./src/UnixDynamicLibraryManager.o \
./src/Win32DynamicLibraryManager.o \
./src/XmlDocument.o \
./src/XmlElement.o \
./src/XmlOutputter.o \
./src/XmlOutputterHook.o 

CPP_DEPS += \
./src/AdditionalMessage.d \
./src/Asserter.d \
./src/BeOsDynamicLibraryManager.d \
./src/BriefTestProgressListener.d \
./src/CompilerOutputter.d \
./src/DefaultProtector.d \
./src/DllMain.d \
./src/DynamicLibraryManager.d \
./src/DynamicLibraryManagerException.d \
./src/Exception.d \
./src/Message.d \
./src/PlugInManager.d \
./src/PlugInParameters.d \
./src/Protector.d \
./src/ProtectorChain.d \
./src/RepeatedTest.d \
./src/ShlDynamicLibraryManager.d \
./src/SourceLine.d \
./src/StringTools.d \
./src/SynchronizedObject.d \
./src/Test.d \
./src/TestAssert.d \
./src/TestCase.d \
./src/TestCaseDecorator.d \
./src/TestComposite.d \
./src/TestDecorator.d \
./src/TestFactoryRegistry.d \
./src/TestFailure.d \
./src/TestLeaf.d \
./src/TestNamer.d \
./src/TestPath.d \
./src/TestPlugInDefaultImpl.d \
./src/TestResult.d \
./src/TestResultCollector.d \
./src/TestRunner.d \
./src/TestSetUp.d \
./src/TestSuccessListener.d \
./src/TestSuite.d \
./src/TestSuiteBuilderContext.d \
./src/TextOutputter.d \
./src/TextTestProgressListener.d \
./src/TextTestResult.d \
./src/TextTestRunner.d \
./src/TypeInfoHelper.d \
./src/UnixDynamicLibraryManager.d \
./src/Win32DynamicLibraryManager.d \
./src/XmlDocument.d \
./src/XmlElement.d \
./src/XmlOutputter.d \
./src/XmlOutputterHook.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++0x -I"D:\Programming\Repos\rpiapi\cppunit-1.12.1\include" -I"D:\Programming\Repos\rpiapi\cppunit-1.12.1\include\cppunit" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


