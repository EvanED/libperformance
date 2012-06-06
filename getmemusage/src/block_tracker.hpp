#ifndef BLOCK_TRACKER_HPP
#define BLOCK_TRACKER_HPP

#include <string>
#include <iosfwd>

#include <boost/scoped_ptr.hpp> // One day, replace with std::unique_ptr

namespace details {
    struct block_tracker_impl;
}

class block_tracker
{
    // Using the pimpl idiom not because of "I want to hide stuff" really,
    // and just because of the amount of platform dependence. I want this
    // header to be common across platforms and free of #ifdef crap.
    boost::scoped_ptr<details::block_tracker_impl> const pimpl;
    
public:
    block_tracker();
    block_tracker(std::string const & desc);

    void start();
    void stop();

    void report() const;
    void report(std::ostream & is) const;

    bool is_running() const;

    ~block_tracker();
};

// Yo emacs!
// Local Variables:
//     c-basic-offset: 4
//     indent-tabs-mode: nil
// End:

#endif
