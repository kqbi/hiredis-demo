//
// Created by kqbi on 2021/1/16.
//

#ifndef HIREDIS_DEMO_REDISPOOL_H
#define HIREDIS_DEMO_REDISPOOL_H

#include "ResourcePool.h"
#include "hiredis/hiredis.h"
#include <memory>

namespace hiredis {
    class RedisConnection {
    public:
        typedef std::shared_ptr<RedisConnection> Ptr;

        RedisConnection(const char *pool_name, const char *server_ip, int server_port, int db_num);

        ~RedisConnection();

        bool Init();

        const char *GetPoolName();

        bool checkStatus();

		bool isInitFlags();

        bool connect();
//        string get(string key);
//        string setex(string key, int timeout, string value);
        std::string Set(std::string key, std::string& value);
//
//        //批量获取
//        bool mget(const vector<string>& keys, map<string, string>& ret_value);
//        // 判断一个key是否存在
//        bool isExists(string &key);
//
//        // Redis hash structure
//        long hdel(string key, string field);
//        string hget(string key, string field);
//        bool hgetAll(string key, map<string, string>& ret_value);
//        long hset(string key, string field, string value);
//
//        long hincrBy(string key, string field, long value);
//        long incrBy(string key, long value);
//        string hmset(string key, map<string, string>& hash);
//        bool hmget(string key, list<string>& fields, list<string>& ret_value);
//
//        //原子加减1
//        long incr(string key);
//        long decr(string key);
//
//        // Redis list structure
//        long lpush(string key, string value);
//        long rpush(string key, string value);
//        long llen(string key);
//        bool lrange(string key, long start, long end, list<string>& ret_value);

    private:
        // CachePool* 		m_pCachePool;
        redisContext *_pContext = 0;
        uint64_t _last_connect_time;
        std::string _pool_name;
        std::string _server_ip;
        int _server_port;
        int _db_num;

        int _cur_conn_cnt;
        int _max_conn_cnt;
		bool _initFlags;
        // list<CacheConn*>	m_free_list;
        //  CThreadNotify		m_free_notify;
    };


    class RedisPool {
    public:
        typedef std::shared_ptr<RedisPool> Ptr;
        typedef ResourcePool<RedisConnection> PoolType;
        typedef std::vector<std::vector<std::string> > SqlRetType;

        RedisPool();

        ~RedisPool();

        static RedisPool &Instance();

        template<typename ...Args>
        void Init(Args &&...arg) {
            _pool.reset(new PoolType(std::forward<Args>(arg)...));
            _pool->obtain();
        }

        RedisConnection::Ptr RedisPool::GetCacheConn() {
            return _pool->obtain();
        }

    private:
        std::shared_ptr<PoolType> _pool;
    };
}

#endif //HIREDIS_DEMO_REDISPOOL_H
