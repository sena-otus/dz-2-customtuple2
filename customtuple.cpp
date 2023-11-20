#include <string>
#include <cassert>
#include <utility>
#include <iostream>

template <typename... Args>
struct Customtuple;

template<typename First, typename... Tail>
struct Customtuple<First, Tail...>
{
    /** Tuple type with const and reference removed from members,
     * it works also with std::remove_reference<T> instead of std::decay<T>,
     * so not really sure, which one to use here */
  using noref_Customtuple = Customtuple<typename std::decay<First>::type, typename std::decay<Tail>::type...>;

    /** use recursion to initialize values */
  template<typename UFirst, typename... UTail>
  explicit Customtuple(UFirst&& first, UTail&&... tail)
    : m_first(std::forward<UFirst>(first)), m_tail(std::forward<UTail>(tail)...)
  {  }

    /** following is not necessary for that example, but may be useful for other cases */
  explicit Customtuple(const noref_Customtuple& other): m_first(other.m_first), m_tail(other.m_tail)  {  }

    /** that one is really important for tie */
  Customtuple&operator=(const noref_Customtuple& other)
  {
    m_first = other.m_first;
    m_tail = other.m_tail;
    return *this;
  }

    /** Opposite of noref_Customtuple, tuple type with added reference to each member
     * need that to allow operator=() to access members of noref_Customtuple */
  using ref_Customtuple = Customtuple<First&, Tail&...>;
  friend ref_Customtuple;

private:
  First m_first;
  Customtuple<Tail...> m_tail;
};

/** need that to stop recursion */
template <>
struct Customtuple<>
{
};


/** again, here std::remove_reference<T> instead of std::decay<T> works too */
template<class... Types>
Customtuple<typename std::decay<Types>::type...> make_Customtuple(Types&&... args)
{
  return Customtuple<typename std::decay<Types>::type...>(std::forward<Types>(args)...);
}


auto getPerson() {
  const std::string name = "Petia";
  const std::string secondName = "Ivanoff";
  const std::size_t age = 23;
  const std::string department = "Sale";
  return make_Customtuple(
    name, secondName, age, department
                         );
}


template<typename... Types>
Customtuple<Types&...> customtie(Types&... args) noexcept
{
    return Customtuple<Types&...>(args...);

      // for the next line to complile, explicit from Customtuple ctor must be removed, so what is less evil?
      //  return {args...};
}


// NOLINTNEXTLINE(hicpp-named-parameter,readability-named-parameter)
int main(int, char *[]) {
  std::string name;
  std::string secondName;
  std::string department;
  std::size_t age{0};
  customtie(name, secondName, age, department) = getPerson();
  assert(name == "Petia");
  assert(secondName == "Ivanoff");
  assert(age == 23);
  assert(department == "Sale");
  return 0;
}
