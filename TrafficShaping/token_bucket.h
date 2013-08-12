/*
 * ***************************************************************************
 * FILENAME:  token_bucket.h
 * DESCRIPTION: 实现了令牌桶流量控制算法
 * HISTORY:    Date        Author         Comment
 *          2013-3-15      zhiguozhang    Created
 * *************************************************************************
*/

#ifndef _TOKEN_BUCKET_H_
#define _TOKEN_BUCKET_H_
#include <sys/time.h>
#include <stdint.h>

class TokenBucket
{
public:
    TokenBucket(double fRateUnitsPerSeconds, double fBucketSize)
        :m_fLastCalcTime(0), m_fBucketWater(0)
    {
        Set(fRateUnitsPerSeconds, fBucketSize);
    }
    
    // 设置速率和桶大小
    void Set(double fRateUnitsPerSeconds, double fBucketSize)
    {
        m_fRateUnitsPerSeconds = fRateUnitsPerSeconds;
        m_fBucketSize = fBucketSize;
    }

    // 按照当前时间更新桶水量
    void UpdateBucketWater()
    {
        struct timeval tvNow;
        gettimeofday(&tvNow, NULL);
        double fNow = (double)tvNow.tv_sec + (double)tvNow.tv_usec / 1000000.0;
        double fWaterFlowed = m_fRateUnitsPerSeconds * ( fNow - m_fLastCalcTime );
        m_fBucketWater += fWaterFlowed;
        if ( m_fBucketWater > m_fBucketSize )
            m_fBucketWater = m_fBucketSize;
        m_fLastCalcTime = fNow;
    }

    // 判断能否发送指定的量，能就从桶中减去这些量，否则调用者就抛弃或Sleep等待到能发的时候再发
    bool CanSend(double fSendUnits)
    {
        UpdateBucketWater();
        if (m_fBucketWater >= fSendUnits)
        {
            m_fBucketWater -= fSendUnits;
            return true;
        }
        else
        {
            return false;
        } 
    }

    // 当调用CanSend()判断可以发送后，实际上又没有发送那么多，就需要调用Compensate()把未发送出去的量补回到桶中，这样流量控制才会准确
    void Compensate(double fSendLeftUnits)
    {
        m_fBucketWater += fSendLeftUnits;
    }
    
private:
    double m_fLastCalcTime; // 最后更新时间
    double m_fBucketWater; // 桶中水量，不能超过m_fBucketSize
    double m_fRateUnitsPerSeconds; // 速率：每秒放多少个
    double m_fBucketSize; // 桶大小
};


#endif

