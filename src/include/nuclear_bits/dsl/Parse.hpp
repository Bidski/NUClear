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

#ifndef NUCLEAR_DSL_PARSE_H
#define NUCLEAR_DSL_PARSE_H

#include "nuclear_bits/dsl/Fusion.hpp"
#include "nuclear_bits/dsl/validation/Validation.hpp"

namespace NUClear {
    namespace dsl {

        template <typename... Sentence>
        struct Parse {
            
            using DSL = Fusion<Sentence...>;
            
            template <typename TFunc, typename... TArgs>
            static inline auto bind(Reactor& r, const std::string& label, TFunc callback, TArgs... args)
            -> decltype(DSL::template bind<Parse<Sentence...>>(r, label, callback, std::forward<TArgs>(args)...)) {
                return DSL::template bind<Parse<Sentence...>>(r, label, callback, std::forward<TArgs>(args)...);
            }
            
            static inline auto get(threading::Reaction& r)
            -> decltype(util::Meta::If<fusion::has_get<DSL>, DSL, fusion::NoOp>::template get<Parse<Sentence...>>(r)) {
                
                return util::Meta::If<fusion::has_get<DSL>, DSL, fusion::NoOp>::template get<Parse<Sentence...>>(r);
            }
            
            static inline bool precondition(threading::Reaction& r) {
                return util::Meta::If<fusion::has_precondition<DSL>, DSL, fusion::NoOp>::template precondition<Parse<Sentence...>>(r);
            }
            
            static inline int priority(threading::Reaction& r) {
                return util::Meta::If<fusion::has_priority<DSL>, DSL, fusion::NoOp>::template priority<Parse<Sentence...>>(r);
            }
            
            static std::unique_ptr<threading::ReactionTask> reschedule(std::unique_ptr<threading::ReactionTask>&& task) {
                return util::Meta::If<fusion::has_reschedule<DSL>, DSL, fusion::NoOp>::template reschedule<DSL>(std::move(task));
            }
            
            static inline void postcondition(threading::ReactionTask& r) {
                util::Meta::If<fusion::has_postcondition<DSL>, DSL, fusion::NoOp>::template postcondition<Parse<Sentence...>>(r);
            }
        };
    }
}

#endif
