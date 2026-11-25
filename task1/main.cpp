#include <iostream>
#include <ostream>

#include "src/Minimize/Moore/Minimize.h"
#include "src/Minimize/Mealy/Minimize.h"
#include "src/MealyToMoore/MealyToMoore.h"
#include "src/MooreToMealy/MooreToMealy.h"
#include "src/New/DeterminizeMoore/DeterminizeMoore.h"
#include "src/New/From/From.h"
#include "src/New/Print/Print.h"
#include "src/Old/Parse/Parse.h"
#include "src/Old/Parse/ParseFromLanguage.h"
#include "src/Regex/OperationTree/ParseRegexToOperationTree.h"
#include "src/Regex/Transfrom/Transform.h"

int main() {
//     auto language = ParseFromLanguage(Side::right, R"(OUTPUT -> 1.Q1 | 0.Q2
// Q1 -> 1.OUTPUT | 0.Q3
// Q2 -> 1.Q3 | 0.OUTPUT
// Q3 -> 1.Q2 | 0.Q1
// )");

//     auto language = MooreParse(R"(type: moore
// start: Q0
//
// states:
// Q0 | Q0 /
// Q1 | Q1 /
// Q2 | Q2 /
// Q3 | Q3 /
// Q4 | Q4 /
// Q5 | Q5 /
// Q6 | Q6 /
// Q7 | Q7 /
// Q8 | Q8 /
// Q9 | Q9 /
// Q10 | Q10 /
// Q11 | Q11 /
// Q12 | Q12 /
// Q13 | Q13 /
// Q14 | Q14 /
// Q15 | Q15 /
// Q16 | Q16 /
// Q17 | Q17 /
// Q18 | Q18 /
// Q19 | Q19 /
// QF | QF / OUTPUT
//
// transitions:
//
// Q0 Q8 EMPTY
// Q0 Q4 EMPTY
// Q0 Q1 EMPTY
//
// Q1 Q1 A
// Q1 Q2 EMPTY
//
// Q2 Q2 B
// Q2 Q3 EMPTY
//
// Q3 Q3 C
// Q3 QF EMPTY
//
// Q4 Q5 A
// Q4 Q6 EMPTY
//
// Q5 Q6 C
//
// Q6 Q7 B
// Q6 Q4 EMPTY
//
// Q7 QF EMPTY
//
// Q8 Q9 EMPTY
// Q8 Q13 EMPTY
// Q8 Q17 EMPTY
//
// Q9 Q10 A
// Q9 Q12 EMPTY
//
// Q10 Q11 B
//
// Q11 Q12 C
//
// Q12 Q9 EMPTY
// Q12 QF EMPTY
//
// Q13 Q14 A
// Q13 Q15 EMPTY
//
// Q14 Q15 B
//
// Q15 Q16 C
// Q15 Q13 EMPTY
//
// Q16 QF EMPTY
//
// Q17 Q17 A
// Q17 Q18 B
//
// Q18 Q19 C
//
// Q19 QF EMPTY
// )");
//     auto moore = From(language);
//     // PrintTable(moore);
//
//     auto determinizedMoore = DeterminizeMoore(moore);
//     Print(determinizedMoore);

    regex::ParseRegexToOperationTree parser{false};
    const auto regex = parser("(abc|a*bc)*(b*ac|c*ab)*");
    //(abc|a*bc)*(b*ac|c*ab)*
    //((ab)*ab|(bc)+)a*bc|(b+c)*
    //((a+b*|a*b+)|a+b+)+|c
    regex::Printable::PrintTree(regex, regex);

    const regex::Transform<MooreTable> transform{};
    auto moore = transform(regex);

    Print(moore);
}
