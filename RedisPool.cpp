//
// Created by kqbi on 2021/1/16.
//

#include "RedisPool.h"

#ifdef _WIN32

#include <Windows.h>
#include <winsock.h>

#else
#include <sys/time.h>
#endif
namespace hiredis {
    RedisConnection::RedisConnection(const char *pool_name, const char *server_ip, int server_port, int db_num)
            : _pool_name(pool_name), _server_ip(server_ip), _server_port(server_port), _db_num(db_num) {
        // 200ms超时
        struct timeval timeout = {0, 200000};
        _pContext = redisConnectWithTimeout(_server_ip.c_str(), _server_port, timeout);
        if (!_pContext || _pContext->err) {
            if (_pContext) {
                redisFree(_pContext);
                _pContext = NULL;
            }
            throw std::exception("redisConnect failed");
        }

        redisReply *reply = (redisReply *) redisCommand(_pContext, "SELECT %d", _db_num);
        if (reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0)) {
            freeReplyObject(reply);
        } else {
            throw std::exception("select cache db %d failed", _db_num);
        }
    }

    RedisConnection::~RedisConnection() {

    }

    const char *RedisConnection::GetPoolName() {
        return _pool_name.c_str();
    }

    bool RedisConnection::checkStatus() {
        bool ret = true;
        do {
            redisReply *reply = (redisReply *) redisCommand(_pContext, "ping");
            if (reply == NULL) {
                ret = false;
            }

            std::shared_ptr<redisReply> autoFree(reply, freeReplyObject);

            if (reply->type != REDIS_REPLY_STATUS) {
                ret = false;
                break;
            };
            if (strcmp(reply->str, "PONG") != 0) {
                ret = false;
                break;
            }
        } while (0);

        if (!ret) {
            if (_pContext)
                redisFree(_pContext);
            ret = connect();
        }

        return ret;
    }

    bool RedisConnection::connect() {
        // 3s 尝试重连一次
        uint64_t cur_time = (uint64_t) time(NULL);
        int expire = _last_connect_time + 3 - cur_time;
        if (expire >= 0) {
            return false;
        }

        _last_connect_time = cur_time;

        // 200ms超时
        struct timeval timeout = {0, 200000};
        _pContext = redisConnectWithTimeout(_server_ip.c_str(), _server_port, timeout);
        if (!_pContext || _pContext->err) {
            if (_pContext) {
                printf("redisConnect failed: %s\n", _pContext->errstr);
                redisFree(_pContext);
                _pContext = NULL;
            } else {
                printf("redisConnect failed\n");
            }

            return false;
        }

        redisReply *reply = (redisReply *) redisCommand(_pContext, "SELECT %d", _db_num);
        if (reply && (reply->type == REDIS_REPLY_STATUS) && (strncmp(reply->str, "OK", 2) == 0)) {
            freeReplyObject(reply);
            return true;
        } else {
            printf("select cache db failed\n");
            return false;
        }
    }


    std::string RedisConnection::Set(std::string key, std::string &value) {
        string ret_value;

        if (!checkStatus()) {
            return ret_value;
        }

        redisReply *reply = (redisReply *) redisCommand(_pContext, "SET %s %s", key.c_str(), value.c_str());
        if (!reply) {
            printf("redisCommand failed:%s", _pContext->errstr);
            redisFree(_pContext);
            _pContext = NULL;
            return ret_value;
        }

        ret_value.append(reply->str, reply->len);
        freeReplyObject(reply);
        return ret_value;
    }

    RedisPool::RedisPool() {

    }

    RedisPool::~RedisPool() {

    }

    RedisPool &RedisPool::Instance() {
        static RedisPool pool;
        return pool;
    }
}