#ifndef HOOK_HPP_RNAM6XF4
#define HOOK_HPP_RNAM6XF4

#include <functional>
#include <utility>

namespace vanetza
{

/**
 * Hook mechanism for realising extension points
 */
template<typename... Args>
class Hook
{
public:
    typedef std::function<void(Args...)> callback_type;

    /**
     * Assign a callback to hook, replaces previously assigned one
     * \param cb A callable used as hook callback, e.g. lambda
     */
    void operator=(callback_type&& cb)
    {
        m_function = std::move(cb);
    }

    /**
     * Execute hook callback if assigned
     * \param Args... various arguments passed to assigned callback
     */
    void operator()(Args... args)
    {
        if (m_function) {
            // that's an arcane syntax, isn't it?
            m_function(std::forward<Args>(args)...);
        }
    }

    void clear()
    {
        m_function = nullptr;
    }

private:
    callback_type m_function;
};

} // namespace vanetza

#endif /* HOOK_HPP_RNAM6XF4 */

