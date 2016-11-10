/* Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "test.h"

#include "arg-parse.h"

#include <assert.h>

namespace testargparse {
namespace {

using namespace argparse;

const std::string g_shortFlag = "-a";
const std::string g_longFlag = "--a";
const std::string g_description = "Simple settable flag without value.";

TestContext::Return testShortFlagNotSet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return TAP_PASS(ctx, "Simple short flag test where flag doesn't set.");
}

TestContext::Return testShortFlagOnlySet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("-a") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (!args[g_shortFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_shortFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    if (&(args[g_shortFlag]) != &(args[g_longFlag]))
        return TAP_FAIL(ctx, "The short and long flag return values not same!");

    if (!args[g_longFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_longFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    return TAP_PASS(ctx, "Simple short flag test.");
}

TestContext::Return testLongFlagNotSet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    return TAP_PASS(ctx, "Simple long flag test where flag doesn't set.");
}

TestContext::Return testLongFlagOnlySet(TestContext* ctx)
{
    char* argv[] = { TAP_CHARS("program"), TAP_CHARS("--a") };
    const int argc = TAP_ARRAY_SIZE(argv);

    ArgParse args;

    args.add(Flag(g_longFlag, g_shortFlag, g_description));

    const bool parseRet = args.parse(argc, argv);

    TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

    TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

    if (!args[g_longFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_longFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    if (&(args[g_shortFlag]) != &(args[g_longFlag]))
        return TAP_FAIL(ctx, "The short and long flag return values not same!");

    if (!args[g_shortFlag].isSet)
        return TAP_FAIL(ctx, "The flag is not set!");

    if (args[g_shortFlag].hasValue)
        return TAP_FAIL(ctx, "Wrong 'hasValue' set to true!");

    return TAP_PASS(ctx, "Simple long flag test.");
}

TestContext::Return testLongAndShortFlagWithValueNotNeeded(TestContext* ctx)
{
    std::string value("--v");

    ArgParse args;

    args.add(Flag("", g_shortFlag, g_description, Value("w", "value")));
    args.add(Flag(g_longFlag, "", g_description, Value("w", "value")));

    struct {
        const std::string givenFlag;
        const std::string definedFlag;
    } testCases[] = {
        { "--a", g_longFlag },
        { std::string("--a=") + value, g_longFlag },
        { "-a", g_shortFlag },
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string givenFlag = testCases[testCase].givenFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string caseName = givenFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenFlag.c_str()), TAP_CHARS(value.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (!args[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (!args[definedFlag].hasValue)
            return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

        if (args[definedFlag].value._isValueNeeded)
            return TAP_FAIL(ctx, caseName + "Wrong '_isValueNeeded'!");

        if (args[definedFlag].value.str != value)
            return TAP_FAIL(ctx, caseName + "Different values!");
    }

    return TAP_PASS(ctx, "Long and short flag tests with not needed value.");
}

TestContext::Return testLongAndShortFlagWithNeededValue(TestContext* ctx)
{
    std::string value("-v");

    ArgParse args;

    args.add(Flag("", g_shortFlag, g_description, Value("", "value")));
    args.add(Flag(g_longFlag, "", g_description, Value("", "value")));

    struct {
        const std::string givenFlag;
        const std::string definedFlag;
        const std::string definedValue;
    } testCases[] = {
        { "--a", g_longFlag, value },
        { std::string("--a=") + value, g_longFlag, "" },
        { "-a", g_shortFlag, value },
    };

    for (size_t testCase = 0; testCase < TAP_ARRAY_SIZE(testCases); ++testCase)
    {
        const std::string givenFlag = testCases[testCase].givenFlag;
        const std::string definedFlag = testCases[testCase].definedFlag;
        const std::string definedValue = testCases[testCase].definedValue;
        const std::string caseName = givenFlag + "|" + definedFlag + " testcase. ";

        char* argv[] = { TAP_CHARS("program"), TAP_CHARS(givenFlag.c_str()), TAP_CHARS(definedValue.c_str()) };
        const int argc = TAP_ARRAY_SIZE(argv);

        const bool parseRet = args.parse(argc, argv);

        TAP_CHECK_PARSER_EXPECTED_RETURN(ctx, (parseRet != true));

        TAP_CHECK_NON_REQUIRED_ERRORS(ctx, args, 0);

        if (!args[definedFlag].isSet)
            return TAP_FAIL(ctx, caseName + "The flag is not set!");

        if (!args[definedFlag].hasValue)
            return TAP_FAIL(ctx, caseName + "Wrong set of 'hasValue'!");

        if (!args[definedFlag].value._isValueNeeded)
            return TAP_FAIL(ctx, caseName + "Wrong '_isValueNeeded'!");

        if (args[definedFlag].value.str != value)
            return TAP_FAIL(ctx, caseName + "Different values!");
    }

    return TAP_PASS(ctx, "Long and short flag tests with not needed value.");
}

} // namespace anonymous

void unitFlagStructTests(TestContext* ctx)
{
    ctx->add(testShortFlagNotSet);
    ctx->add(testShortFlagOnlySet);
    ctx->add(testLongFlagNotSet);
    ctx->add(testLongFlagOnlySet);
    ctx->add(testLongAndShortFlagWithValueNotNeeded);
    ctx->add(testLongAndShortFlagWithNeededValue);
}

} // namespace testargparse