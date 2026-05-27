/*
 * thread.hpp
 *
 *  Created on: 28 мая 2026 г.
 *      Author: mastersan
 */

#ifndef SRC_HELPERS_THREAD_HPP_
#define SRC_HELPERS_THREAD_HPP_

#include <pthread.h>
#include <string>

namespace App
{
namespace GHelpers
{
namespace Thread
{

static inline
void set_self_name(
        std::string name
)
{
    static const size_t thread_name_size_max = 15;
    name.resize(thread_name_size_max);
    const pthread_t current_thread = pthread_self();
    pthread_setname_np(
            current_thread,
            name.c_str()
    );
}

} /* namespace Thread */
} /* namespace GHelpers */
} /* namespace App */


#endif /* SRC_HELPERS_THREAD_HPP_ */
