/*
 * ***************************************************************************
 * FILENAME:  token_bucket.h
 * DESCRIPTION: ʵ��������Ͱ���������㷨
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
    
    // �������ʺ�Ͱ��С
    void Set(double fRateUnitsPerSeconds, double fBucketSize)
    {
        m_fRateUnitsPerSeconds = fRateUnitsPerSeconds;
        m_fBucketSize = fBucketSize;
    }

    // ���յ�ǰʱ�����Ͱˮ��
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

    // �ж��ܷ���ָ���������ܾʹ�Ͱ�м�ȥ��Щ������������߾�������Sleep�ȴ����ܷ���ʱ���ٷ�
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

    // ������CanSend()�жϿ��Է��ͺ�ʵ������û�з�����ô�࣬����Ҫ����Compensate()��δ���ͳ�ȥ�������ص�Ͱ�У������������ƲŻ�׼ȷ
    void Compensate(double fSendLeftUnits)
    {
        m_fBucketWater += fSendLeftUnits;
    }
    
private:
    double m_fLastCalcTime; // ������ʱ��
    double m_fBucketWater; // Ͱ��ˮ�������ܳ���m_fBucketSize
    double m_fRateUnitsPerSeconds; // ���ʣ�ÿ��Ŷ��ٸ�
    double m_fBucketSize; // Ͱ��С
};


#endif

