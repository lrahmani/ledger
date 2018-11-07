//------------------------------------------------------------------------------
//
//   Copyright 2018 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include "core/json/document.hpp"
#include "core/json/exceptions.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

using namespace fetch::json;
using namespace fetch::byte_array;

std::string n_TEST_CASES[] = {

    {"[1 true]"},  // 0
    {"[a�]"},
    {"[\"\": 1]"},
    {"[\"\"],"},
    {"[,1]"},
    {"[1,,2]"},
    {"[\"x\",,]"},
    {"[\"x\"]]"},
    {"[\"\",]"},
    {"[\"x\""},
    {"[x"},  // 10
    {"[3[4]]"},
    {"[�]"},
    {"[1:2]"},
    {"[,]"},
    {"[-]"},
    {"[   , \"\"]"},
    {"[\"a\",\n4\n,1,"},
    {"[1,]"},
    {"[1,,]"},
    {"[\"a\"\f]"},  // 20
    {"[*]"},
    {"[\"\""},
    {"[1,"},
    {"[1,\n1\n,1"},
    {"[{}"},
    {"[fals]"},
    {"[nul]"},
    {"[tru]"},
    {"123"},
    {"[++1234]"},  // 30
    {"[+1]"},
    {"[+Inf]"},
    {"[-01]"},
    {"[-1.0.]"},
    {"[-2.]"},
    {"[-NaN]"},
    {"[.-1]"},
    {"[.2e-3]"},
    {"[0.1.2]"},
    {"[0.3e+]"},  // 40
    {"[0.3e]"},
    {"[0.e1]"},
    {"[0E+]"},
    {"[0E]"},
    {"[0e+]"},
    {"[0e]"},
    {"[1.0e+]"},
    {"[1.0e-]"},
    {"[1.0e]"},
    {"[1 000.0]"},  // 50
    {"[1eE2]"},
    {"[2.e+3]"},
    {"[2.e-3]"},
    {"[9.e+]"},
    {"[Inf]"},
    {"[NaN]"},
    {"[１]"},
    {"[1+2]"},
    {"[0x1]"},
    {"[0x42]"},  // 60
    {"[Infinity]"},
    {"[0e+-1]"},
    {"[-123.123foo]"},
    {"[123�]"},
    {"[1e1�]"},
    {"[0�]"},
    {"[-Infinity]"},
    {"[-foo]"},
    {"[- 1]"},
    {"[-012]"},  // 70
    {"[-.123]"},
    {"[-1x]"},
    {"[1ea]"},
    {"[1e�]"},
    {"[1.]"},
    {"[.123]"},
    {"[1.2a-3]"},
    {"[1.8011670033376514H-308]"},
    {"[012]"},
    {"[\"x\", truth]"},  // 80
    {"{[: \"x\"}"},
    {"{\"x\", null}"},
    {"{\"x\"::\"b\"}"},
    {"{🇨🇭}"},
    {"{\"a\":\"a\" 123}"},
    {"{key: 'value'}"},
    {"{\"�\":\"0\",}"},
    {"{\"a\" b}"},
    {"{:\"b\"}"},
    {"{\"a\" \"b\"}"},  // 90
    {"{\"a\":"},
    {"{\"a\""},
    {"{1:1}"},
    {"{9999E9999:1}"},
    {"{null:null,null:null}"},
    {"{\"id\":0,,,,,}"},
    {"{'a':0}"},
    {"{\"id\":0,}"},
    {"{\"a\":\"b\"}/**/"},
    {"{\"a\":\"b\"}/**//"},  // 100
    {"{\"a\":\"b\"}//"},
    {"{\"a\":\"b\"}/"},
    {"{\"a\":\"b\",,\"c\":\"d\"}"},
    {"{a: \"b\"}"},
    {"{\"a\":\"a"},
    {"{ \"foo\" : \"bar\", \"a\" }"},
    {"{\"a\":\"b\"}#"},
    {" "},
    {"[é]"},
    {"[\"\\\"]"},  // 110
    {"[\"\x00\"]"},
    {"[\"\\\\\"]"},
    {"[\"\\\"]"},
    {"[\"\a\"]"},
    {"[\u0020\"asd\"]"},
    {"[\n]"},
    {"\""},
    {"['single quote']"},
    {"abc"},
    {"[\"\\"},  // 120
    {"[\"aa\"]"},
    {"[\"new\nline\"]"},
    {"[\"	\"]"},
    {"\"\"x"},
    {"[⁠]"},
    {""},
    {"<.>"},
    {"[<null>]"},
    {"[1]x"},
    {"[1]]"},  // 130
    {"[\"asd]"},
    {"aå"},
    {"[True]"},
    {"1]"},
    {"{\"x\": true,"},
    {"[][]"},
    {"]"},
    {"�{}"},
    {"�"},
    {"["},  // 140
    {""},
    {"[]"},
    {"2@"},
    {"{}}"},
    {"{\"\":"},
    {"{\"a\":/*comment*/\"b\"}"},
    {"{\"a\": true} \"x\""},
    {"['"},
    {"[,"},
    {"[{"},  // 150
    {"[\"a"},
    {"[\"a\""},
    {"{"},
    {"{]"},
    {"{,"},
    {"{["},
    {"{\"a"},
    {"{'a'"},
    {"�"},
    {"*"},  // 160
    {"{\"a\":\"b\"}#{}"},
    {"[\u000A\"\"]"},
    {"[1"},
    {"[ false, nul"},
    {"[ true, fals"},
    {"[ false, tru"},
    {"{\"asd\":\"asd\""},
    {"å"},
    {"[⁠]"},
    {"[]"},  // 170
    {"[\"\\	\\\"]"},
    {"[\"\\🌀\"]"},
    {"[\"\\�\"]"},
    {"[\"\\{[\"\\{[\"\\{[\"\\{"},
    //{"[\"\uD800\\\"]"},
    //{"[\"\uD800\u\"]"},
    //{"[\"\uD800\u1\"]"},
    //{"[\"\uD800\u1x\"]"},
    //{"[\"\u00A\"]"},
    //{"[\"\uD834\uDd\"]"},          // 180
    //{"[\"\uD800\uD800\x\"]"},
    //{"[\"\u�\"]"},
    //{"[\"\uqqqq\"]"},
    //{"\"\UA66D\""},

};

std::string y_TEST_CASES[] = {

    {"[[]   ]"},  // 0
    {"[\"\"]"},
    {"[]"},
    {"[\"a\"]"},
    {"[false]"},
    {"[null, 1, \"1\", {}]"},
    {"[null]"},
    {"[1\n]"},
    {"[1]"},
    {"[1,null,null,null,2]"},
    {"[2]"},  // 10
    {"[123e65]"},
    {"[0e+1]"},
    {"[0e1]"},
    {"[ 4]"},
    {"[-0.000000000000000000000000000000000000000000000000000000000000000000000000000001]"},
    {"[20e1]"},
    {"[-0]"},
    {"[-123]"},
    {"[-1]"},
    {"[-0]"},  // 20
    {"[1E22]"},
    {"[1E-2]"},
    {"[1E+2]"},
    {"[123e45]"},
    {"[123.456e78]"},
    {"[1e-2]"},
    {"[1e+2]"},
    {"[123]"},
    {"[123.456789]"},
    {"{\"asd\":\"sdf\", \"dfg\":\"fgh\"}"},  // 30
    {"{\"asd\":\"sdf\"}"},
    {"{\"a\":\"b\",\"a\":\"c\"}"},
    {"{\"a\":\"b\",\"a\":\"b\"}"},
    {"{}"},
    {"{\"\":0}"},
    {"{\"foo\u0000bar\": 42}"},
    {"{ \"min\": -1.0e+28, \"max\": 1.0e+28 }"},
    {"{\"x\":[{\"id\": \"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}], \"id\": "
     "\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\"}"},
    {"{\"a\":[]}"},
    {"{\"title\":\"\u041f\u043e\u043b\u0442\u043e\u0440\u0430 "
     "\u0417\u0435\u043c\u043b\u0435\u043a\u043e\u043f\u0430\" }"},  // 40
    {"{\n\"a\": \"b\"\n}"},
    {"[\"\u0060\u012a\u12AB\"]"},
    {"[\"\\u0000\"]"},
    {"[\"\"\"]"},
    {"[\"\\a\"]"},
    {"[\"\\n\"]"},
    {"[\"\u0012\"]"},
    {"[\"\uFFFF\"]"},
    {"[\"asd\"]"},
    {"[ \"asd\"]"},  // 50
    {"[\"new\u00A0line\"]"},
    {"[\"􏿿\"]"},
    {"[\"￿\"]"},
    {"[\"\u002c\"]"},
    {"[\"\u0000\"]"},
    {"[\"π\"]"},
    {"[\"𛿿\"]"},
    {"[\"asd \"]"},
    {"\" \""},
    {"[\"\u0821\"]"},  // 60
    {"[\"\u0123\"]"},
    {"[\" \"]"},
    {"[\" \"]"},
    {"[\"\u0061\u30af\u30EA\u30b9\"]"},
    {"[\"new\u000Aline\"]"},
    {"[\"\"]"},
    {"[\"\uA66D\"]"},
    {"[\"\u005C\"]"},
    {"[\"⍂㈴⍂\"]"},
    {"[\"\u200B\"]"},  // 70
    {"[\"\u2064\"]"},
    {"[\"\uFDD0\"]"},
    {"[\"\uFFFE\"]"},
    {"[\"\u0022\"]"},
    {"[\"€𝄞\"]"},
    {"[\"aa\"]"},
    {"false"},
    {"42"},
    {"-0.1"},
    {"null"},  // 80
    {"\"asd\""},
    {"true"},
    {"\"\""},
    {"[\"a\"]"},
    {"[true]"},
    {"[]"},
    {"[]"},
    {"\"\"x"},
    //{"[\"\uD801\udc37\"]"},
    //{"[\"\ud83d\ude39\ud83d\udc8d\"]"},          // 90
    //{"[\"\\\"\\\/\b\f\n\r\t\"]"},
    //{"[\"\uDBFF\uDFFF\"]"},
    //{"[\"\uD834\uDd1e\"]"},
    //{"[\"\uDBFF\uDFFE\"]"},
    //{"[\"\uD83F\uDFFE\"]"},

};

class n_JsonTests : public ::testing::TestWithParam<std::string>
{
protected:
};

TEST_P(n_JsonTests, n_CheckParsing)
{
  std::string const &config = GetParam();
  JSONDocument       doc;
  EXPECT_THROW(doc.Parse(config), fetch::json::JSONParseException);
}

class y_JsonTests : public ::testing::TestWithParam<std::string>
{
protected:
};

TEST_P(y_JsonTests, y_CheckParsing)
{
  std::string const &config = GetParam();
  JSONDocument       doc;
  EXPECT_NO_THROW(doc.Parse(config));
}

INSTANTIATE_TEST_CASE_P(ParamBased, n_JsonTests, testing::ValuesIn(n_TEST_CASES), );
INSTANTIATE_TEST_CASE_P(ParamBased, y_JsonTests, testing::ValuesIn(y_TEST_CASES), );
