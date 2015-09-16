/*
 * Copyright (C) 2013 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef NUCLEAR_DSL_FUSION_HASRESCHEDULE_H
#define NUCLEAR_DSL_FUSION_HASRESCHEDULE_H

#include "nuclear_bits/threading/ReactionTask.hpp"
#include "nuclear_bits/dsl/fusion/NoOp.hpp"

namespace NUClear {
    namespace dsl {
        namespace fusion {
            
            template<typename T>
            struct has_reschedule {
            private:
                typedef std::true_type yes;
                typedef std::false_type no;
                
                template<typename U> static auto test(int) -> decltype(U::template reschedule<ParsedNoOp>(std::declval<std::unique_ptr<threading::ReactionTask>>()), yes());
                template<typename> static no test(...);
                
            public:
                static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
            };
        }
    }
}

#endif
