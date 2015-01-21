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

#ifndef NUCLEAR_DSL_FUSION_HASBIND_H
#define NUCLEAR_DSL_FUSION_HASBIND_H

#include "nuclear_bits/util/MetaProgramming.h"
#include "nuclear_bits/threading/ReactionHandle.h"
#include "nuclear_bits/dsl/fusion/NoOp.h"

namespace NUClear {
    namespace dsl {
        namespace fusion {
            
            template <typename T>
            struct has_bind {
            private:
                typedef std::true_type yes;
                typedef std::false_type no;
                
                template <typename R, typename F, typename... A>
                static yes test_func(R(*)(Reactor&, const std::string&, F, A...));
                static no test_func(...);
                
                template <typename U> static auto test(int) -> decltype(test_func(U::template bind<ParsedNoOp, std::function<void ()>>));
                template <typename> static no test(...);
                
            public:
                static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
            };
        }
    }
}

#endif
