#include "PropertyTree.h"
#include "PropertyTreeReader.h"
#include "PropertyTreeWriter.h"

#include "../json/json.h"

#include <iostream>
#include <cassert>
#include <fstream>

struct DataPlain
{
    int x, y;

} dataPlain;

template<>
struct PropertyTreeOutputConverter<DataPlain>
{
    PropertyTree operator ()(const DataPlain& data)
    {
        PropertyTree tree;
        tree.put("x", data.x);
        tree.put("y", data.y);

        return tree;
    }
};

template<>
struct PropertyTreeInputConverter<DataPlain>
{
    DataPlain operator ()(const PropertyTree& tree)
    {
        DataPlain data;
        data.x = tree.find("x").get<int>();
        data.y = tree.find("y").get<int>();

        return data;
    }
};

void example()
{
    // R�czne wstawianie warto�ci do drzewa.
    PropertyTree tree;
    tree.put("firstName", "John");
    tree.put("lastName", "Smith");
    tree.put("isAlive", true);
    tree.put("age", 25);

    PropertyTree address;
    address.put("streetAddress", "21 2nd Street");
    address.put("city", "New York");
    address.put("state", "NY");
    address.put("postalCode", "10021 - 3100");

    tree.put("address", address);

    std::vector<PropertyTree> phones(2);
    phones[0].put("type", "home");
    phones[0].put("number", "212 555 - 1234");
    phones[1].put("type", "office");
    phones[1].put("number", "646 555 - 4567");

    tree.put("phoneNumbers", phones);
    tree.put("children", std::vector<std::nullptr_t>());
    tree.put("spouse", nullptr);

    // Zapisywanie drzewa w postaci JSON do strumienia.
    WriteJson(tree, std::cout);

    // Powy�sze drzewo zapisane w postaci JSON.
    std::string input = "{                                      \
                         \"firstName\":\"John\",                \
                         \"lastName\":\"Smith\",                \
                         \"isAlive\":true,                      \
                         \"age\": 25,                           \
                         \"address\": {                         \
                           \"streetAddress\": \"21 2nd Street\",\
                           \"city\": \"New York\",              \
                           \"state\": \"NY\",                   \
                           \"postalCode\": \"10021 - 3100\"     \
                         },                                     \
                         \"phoneNumbers\": [                    \
                           {                                    \
                             \"type\": \"home\",                \
                             \"number\": \"212 555 - 1234\"     \
                           },                                   \
                           {                                    \
                             \"type\": \"office\",              \
                             \"number\": \"646 555 - 4567\"     \
                           }                                    \
                         ],                                     \
                         \"children\": [],                      \
                         \"spouse\": null                       \
                         }";
    std::istringstream istr(input);
    
    // Wczytanie drzewa w postaci JSON ze strumienia.
    PropertyTree another;
    ReadJson(another, istr);

    // Warto�ci obu drzew powinny by� jednakowe.
    assert(tree == another);

    /// Konwersja z PropertyTree na Json::Object
    auto json = ToJsonObject(another);
    std::ostringstream output;
    WriteJson(another, output, false);

    // Warto�ci std::string nie s� bezpo�rednio por�wnywalne, ze wzgl�du na to, �e Json::Object nie zachowuje kolejno�ci p�l.
    // assert(json.serialize() == output.str());
    // Aby udowodni� poprawno�� konwersji, odczyt nast�pi bezpo�renio z warto�ci json.serialize().
    istr = std::istringstream(json.serialize());
    another = PropertyTree();
    ReadJson(another, istr);
    auto json2 = ToJsonObject(another);
    output = std::ostringstream();
    WriteJson(another, output, false);
    assert(json2.serialize() == output.str());
    assert(json.serialize() == output.str());

    /// Przyk�ad zamiany struktury danych na drzewo.
    /**
     * Przypadek zaimplementowanych funkcji serialize() i deserialize().
     * Mo�liwy przypadek u�ycia, gdy serializowane warto�ci s� prywatne.
     */
    struct DataSerializable
    {
        int x, y;

        PropertyTree serialize() const
        {
            PropertyTree tree;
            tree.put("x", x);
            tree.put("y", y);

            return tree;
        }

        void deserialize(const PropertyTree& tree)
        {
            x = tree.find("x").get<int>();
            y = tree.find("y").get<int>();
        }

    } dataSerializable;
    
    dataSerializable.x = 1;
    dataSerializable.y = 2;
    tree = PropertyTree();
    tree.put("data", dataSerializable);
    WriteJson(tree, std::cout);
    auto otherSerializable = tree.find("data").get<DataSerializable>();
    assert(dataSerializable.x == otherSerializable.x && dataSerializable.y == otherSerializable.y);

    /// Przypadek specjalizacji domy�lnego konwertera.
    /**
     * W sytuacji, gdy serializowanej klasy nie da si� zmieni�.
     */
    DataPlain dataPlain;
    dataPlain.x = 1;
    dataPlain.y = 2;
    tree = PropertyTree();
    tree.put("data", dataPlain);
    WriteJson(tree, std::cout);
    auto otherPlain = tree.find("data").get<DataPlain>();
    assert(dataPlain.x == otherPlain.x && dataPlain.y == otherPlain.y);

    /// Przypadek w�asnej funkcji konwertuj�cej.
    tree = PropertyTree();
    tree.put("data", dataPlain, [](const DataPlain& data)
    {
        PropertyTree tree;
        tree.put("x", data.x + 1); // w celu rozr�nienia od specjalizacji implementacji domy�lnej.
        tree.put("y", data.y + 1);
        return tree;
    }
    );
    WriteJson(tree, std::cout);
    otherPlain = DataPlain();
    otherPlain = tree.find("data").get<DataPlain>([](const PropertyTree& tree)
    {
        DataPlain data;
        data.x = tree.find("x").get<int>();
        data.y = tree.find("y").get<int>();

        return data;
    }
    );

    assert(dataPlain.x + 1 == otherPlain.x && dataPlain.y + 1 == otherPlain.y);

}
