#include <QApplication>
#include <QDebug>

#include <boost/version.hpp>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << BOOST_LIB_VERSION;
    qDebug() << BOOST_VERSION;

    // error code
    boost::system::error_code ec;
    std::string hostname = boost::asio::ip::host_name(ec);
    std::cout << hostname << std::endl;
    std::cout << ec.value() << std::endl;

    // date time
    boost::gregorian::date d = boost::gregorian::day_clock::universal_day();
    std::cout << d.year() << std::endl;
    std::cout << d.month() << std::endl;
    std::cout << d.day() << std::endl;

    d = boost::gregorian::date_from_iso_string("20100131");
    std::cout << d.year() << std::endl;
    std::cout << d.month() << std::endl;
    std::cout << d.day() << std::endl;

    return a.exec();
}
