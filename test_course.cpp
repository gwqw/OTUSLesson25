#define BOOST_TEST_MODULE join_server_test_module
#include <boost/test/unit_test.hpp>

#include "database.h"
#include "command.h"
#include "command_handler.h"

using namespace std;

void createExampleDB(DataBase& db);

BOOST_AUTO_TEST_SUITE(database_test_suite)

	BOOST_AUTO_TEST_CASE(test_database_op_example) {
        DataBase db;

        db.insert("A", 0, "lean");
        db.insert("A", 1, "sweater");
        db.insert("A", 2, "frank");
        db.insert("A", 3, "violation");
        db.insert("A", 4, "quality");
        db.insert("A", 5, "precision");
        db.insert("B", 3, "proposal");
        db.insert("B", 4, "example");
        db.insert("B", 5, "lake");
        db.insert("B", 6, "flour");
        db.insert("B", 7, "wonder");
        db.insert("B", 8, "selection");
        {
            auto res = db.intersection();
            ResponseTable expected = {{3, {"violation", "proposal"}},
                                      {4, {"quality",   "example"}},
                                      {5, {"precision", "lake"}}};
            BOOST_CHECK(res == expected);
        }
        {
            auto res = db.symmetric_difference();
            ResponseTable expected = {{0, {"lean",    ""}},
                                      {1, {"sweater", ""}},
                                      {2, {"frank",   ""}},
                                      {6, {"",        "flour"}},
                                      {7, {"",        "wonder"}},
                                      {8, {"",        "selection"}},
            };
            BOOST_CHECK(res == expected);
        }
	}

    BOOST_AUTO_TEST_CASE(test_database_insert) {
        {
            DataBase db;
            bool res = db.insert("A", 0, "lean");
            Table::table_t expected{{0, "lean"}};
            BOOST_CHECK(res);
            {
                auto table = db.getTable("A").getTable();
                BOOST_CHECK(table == expected);
            }
            res = db.insert("A", 0, "lean");
            BOOST_CHECK(!res);
            {
                auto table = db.getTable("A").getTable();
                BOOST_CHECK(table == expected);
            }
        }
    }

    BOOST_AUTO_TEST_CASE(test_database_truncate) {
        {
            DataBase db;
            db.insert("A", 0, "lean");
            db.truncate("A");
            auto table = db.getTable("A").getTable();
            Table::table_t expected;
            BOOST_CHECK(table == expected);
        }
        {
            DataBase db;
            db.insert("A", 0, "lean");
            db.insert("A", 1, "test");
            db.truncate("A");
            auto table = db.getTable("A").getTable();
            Table::table_t expected;
            BOOST_CHECK(table == expected);
        }
        {
            DataBase db;
            db.truncate("A");
            auto table = db.getTable("A").getTable();
            Table::table_t expected;
            BOOST_CHECK(table == expected);
        }
    }


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(input_command_test_suite)

    BOOST_AUTO_TEST_CASE(test_insert_command_parser) {
        {
            auto command_holder = parseCommandFrom("INSERT A 0 lean"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::INSERT);
            auto &cmd = static_cast<InsertCommand &>(*command_holder);
            BOOST_CHECK(cmd.table_name == "A");
            BOOST_CHECK(cmd.id == 0);
            BOOST_CHECK(cmd.name == "lean");
        }
        {
            auto command_holder = parseCommandFrom("INSERT A 0"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
        {
            auto command_holder = parseCommandFrom("INSERT A"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
        {
            auto command_holder = parseCommandFrom("INSERT"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
        {
            auto command_holder = parseCommandFrom("INSERT A zero lean"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
    }

    BOOST_AUTO_TEST_CASE(test_truncate_command_parser) {
        auto command_holder = parseCommandFrom("TRUNCATE table"sv);
        BOOST_CHECK(command_holder->com_type_ == CommandType::TRUNCATE);
        auto& cmd = static_cast<TruncateCommand&>(*command_holder);
        BOOST_CHECK(cmd.table_name == "table");
    }

    BOOST_AUTO_TEST_CASE(test_intersection_command_parser) {
        auto command_holder = parseCommandFrom("INTERSECTION"sv);
        BOOST_CHECK(command_holder->com_type_ == CommandType::INTERSECTION);
    }

    BOOST_AUTO_TEST_CASE(test_symmetric_difference_command_parser) {
        auto command_holder = parseCommandFrom("SYMMETRIC_DIFFERENCE"sv);
        BOOST_CHECK(command_holder->com_type_ == CommandType::SYMMETRIC_DIFFERENCE);
    }

    BOOST_AUTO_TEST_CASE(test_unknown_command_parser) {
        {
            auto command_holder = parseCommandFrom("INSURT"sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
        {
            auto command_holder = parseCommandFrom(""sv);
            BOOST_CHECK(command_holder->com_type_ == CommandType::UNKNOWN);
        }
    }


BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(response_test_suite)

    BOOST_AUTO_TEST_CASE(test_insert_response) {
        DataBase db;
        {
            auto command_holder = parseCommandFrom("INSERT A 0 lean"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::INSERT);
            auto &cmd = static_cast<InsertResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "OK\n");
        }
        {
            auto command_holder = parseCommandFrom("INSERT A 0 understand"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::INSERT);
            auto &cmd = static_cast<InsertResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "ERR duplicate 0\n");
        }
    }

    BOOST_AUTO_TEST_CASE(test_truncate_response) {
        DataBase db;
        {
            auto command_holder = parseCommandFrom("TRUNCATE A"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::TRUNCATE);
            auto &cmd = static_cast<TruncateResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "OK\n");
        }
        {
            auto resp = getResponseCommand(parseCommandFrom("INSERT A 0 understand"sv));
            auto &cmd = static_cast<InsertResponse &>(*resp);
            cmd.runCommand(db);
        }
        {
            auto command_holder = parseCommandFrom("TRUNCATE A"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::TRUNCATE);
            auto &cmd = static_cast<TruncateResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "OK\n");
        }
        {
            auto command_holder = parseCommandFrom("INSERT A 0 understand"sv);
            auto resp = getResponseCommand(move(command_holder));
            auto &cmd = static_cast<InsertResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "OK\n");
        }
    }

    BOOST_AUTO_TEST_CASE(test_intersection_response) {
        DataBase db;
        createExampleDB(db);
        {
            auto command_holder = parseCommandFrom("INTERSECTION"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::INTERSECTION);
            auto &cmd = static_cast<IntersectionResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res ==
              "3,violation,proposal\n"
              "4,quality,example\n"
              "5,precision,lake\n"
              "OK\n");
        }
        db.truncate("A");
        {
            auto command_holder = parseCommandFrom("INTERSECTION"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::INTERSECTION);
            auto &cmd = static_cast<IntersectionResponse &>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res == "OK\n");
        }
    }

    BOOST_AUTO_TEST_CASE(test_symmetric_difference_response) {
        DataBase db;
        createExampleDB(db);
        {
            auto command_holder = parseCommandFrom("SYMMETRIC_DIFFERENCE"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::SYMMETRIC_DIFFERENCE);
            auto &cmd = static_cast<SymmetricDifferenceResponse&>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res ==
                               "0,lean,\n"
                               "1,sweater,\n"
                               "2,frank,\n"
                               "6,,flour\n"
                               "7,,wonder\n"
                               "8,,selection\n"
                               "OK\n"
            );
        }
        db.truncate("A");
        {
            auto command_holder = parseCommandFrom("SYMMETRIC_DIFFERENCE"sv);
            auto resp = getResponseCommand(move(command_holder));
            BOOST_CHECK(resp->cmd_type_ == CommandType::SYMMETRIC_DIFFERENCE);
            auto &cmd = static_cast<SymmetricDifferenceResponse&>(*resp);
            auto res = cmd.runCommand(db);
            BOOST_CHECK(res ==
                               "3,,proposal\n"
                               "4,,example\n"
                               "5,,lake\n"
                               "6,,flour\n"
                               "7,,wonder\n"
                               "8,,selection\n"
                               "OK\n"
            );
        }
    }


BOOST_AUTO_TEST_SUITE_END()


void createExampleDB(DataBase& db) {
    db.insert("A", 0, "lean");
    db.insert("A", 1, "sweater");
    db.insert("A", 2, "frank");
    db.insert("A", 3, "violation");
    db.insert("A", 4, "quality");
    db.insert("A", 5, "precision");
    db.insert("B", 3, "proposal");
    db.insert("B", 4, "example");
    db.insert("B", 5, "lake");
    db.insert("B", 6, "flour");
    db.insert("B", 7, "wonder");
    db.insert("B", 8, "selection");
}