#pragma once

namespace threads
{
void init(int verbose, int num_threads);

int get_num_threads();
int get_max_threads();
int get_thread_num();

} // namespace threads


namespace opencl
{
    void init(int verbose, int device = 0);
} // end namespace opencl
