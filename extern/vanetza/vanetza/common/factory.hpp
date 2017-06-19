#ifndef FACTORY_HPP_QLKNHPWZ
#define FACTORY_HPP_QLKNHPWZ

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace vanetza
{

/**
 * Factory for a group of classes implementing T
 */
template<typename T, typename... Args>
class Factory
{
public:
    using Result = std::unique_ptr<T>;
    using Function = std::function<Result(Args...)>;

    Factory() : m_default(m_functions.end())
    {
    }

    /**
     * Create an instance of T using a named implementation
     * \param name of wanted implementation
     * \return created instance or nullptr if not found
     */
    Result create(const std::string& name, Args... args) const
    {
        std::unique_ptr<T> obj;
        auto found = m_functions.find(name);
        if (found != m_functions.end()) {
            obj = found->second(std::forward<Args>(args)...);
        }
        return obj;
    }

    /**
     * Create object using default implementation
     * \return created instance or nullptr if no default is configured
     */
    Result create(Args... args) const
    {
        std::unique_ptr<T> obj;
        if (m_default != m_functions.end()) {
            obj = m_default->second(std::forward<Args>(args)...);
        }
        return obj;
    }

    /**
     * Add an implementation to factory
     * \param name of implementation
     * \param f function creating a new instance of this implementation
     * \return true if added successfully, i.e. no previous addition with same name
     */
    bool add(const std::string& name, Function f)
    {
        return m_functions.emplace(name, std::move(f)).second;
    }

    /**
     * Set default implementation
     * \param name selected default implementation
     * \return true if an implementation exists with selected name
     */
    bool configure_default(const std::string& name)
    {
        m_default = m_functions.find(name);
        return m_default != m_functions.end();
    }

private:
    using map_type = std::map<std::string, Function>;
    map_type m_functions;
    typename map_type::const_iterator m_default;
};

} // namespace vanetza

#endif /* FACTORY_HPP_QLKNHPWZ */

