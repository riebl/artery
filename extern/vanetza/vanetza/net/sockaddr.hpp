#ifndef SOCKADDR_HPP_YPVTVXXP
#define SOCKADDR_HPP_YPVTVXXP

struct sockaddr_ll;

namespace vanetza
{

class MacAddress;

void assign(sockaddr_ll&, const MacAddress&);

} // namespace vanetza

#endif /* SOCKADDR_HPP_YPVTVXXP */

