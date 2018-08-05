#ifndef DEL_CODE__H__
#define DEL_CODE__H__

#if 0
void TrafficAlg::process()
{
	LasterData LMS_data_1 = m_vCache.dequeue();
	LasterData LMS_data_2 = m_qCache.dequeue();

	if(!m_qBackground->isReady() || !m_vBackground->isReady()){

		return;
	}

	quint64 Time_Vertical = LMS_data_1.timestamp();

	int i;
	int j;

	int32 l_32tmp,l_tmp1,TempVaule1,TempVaule2;
	uint16 l_32tmp2;
	PointStruct l_u16PosVect[POINTSET_CNT];	//��Ų�����λ����Ϣ
	uint16 l_u16index,l_u16tmp;

	uint16 l_leftPt = 0,l_rightPt = 0;	//��,�ҵ���
	int32 l_leftX = 0,l_rightX = 0;    //����x����
	uint16 l_leftPt_absV = 0,l_rightPt_absV = 0;
	uint16 l_leftPt_absI = 0,l_rightPt_absI = 0;

	PointSet l_FrameInfo1,l_FrameInfo2;
	//֡���ݽ��з��г�����,��¼ÿ�����������λ����Ϣ
	LMS_data_1.splitArea(TrafficSettings::instance()->getLasterV(),m_vBackground,&l_FrameInfo1);

	LMS_data_2.splitArea(TrafficSettings::instance()->getLasterQ(),m_qBackground,&l_FrameInfo2);

	//���㳵���ĳ�����ʱ�������������ʼλ������������λ�ô��ڸ�����ƫ��
	//�ʶ����ڴ˴�������Ϊ���ʣ������仯��������һ�ξ�OK��
	if(TrafficSettings::instance()->getLasterV().LmsObstacleleft() && !g_leftXOffset)
	{
		l_u16tmp = abs(TrafficSettings::instance()->getLasterV().LmsPtMid() - TrafficSettings::instance()->getLasterV().LmsObstacleleft());
		g_leftXOffset = MathHelper::Sin(LMS_data_1[TrafficSettings::instance()->getLasterV().LmsObstacleleft()],l_u16tmp);
		g_leftXOffset *=(-1);
	}

	if(TrafficSettings::instance()->getLasterV().LmsObstacleright() && !g_rightXOffset)
	{
		l_u16tmp = abs(TrafficSettings::instance()->getLasterV().LmsPtMid() - TrafficSettings::instance()->getLasterV().LmsObstacleright());
		g_rightXOffset = MathHelper::Sin(LMS_data_1[TrafficSettings::instance()->getLasterV().LmsObstacleright()],l_u16tmp);
	}

	l_leftPt_absV = LMS_data_1.GetLeft();
	l_rightPt_absV = LMS_data_1.GetRight();


	//����������в�ֵĹ����а����м��������
	//1����ǵ���ڵ�����£�����������Ƚϴ�
	//2��������ʻʱ����������һ�ߣ���Ҫ���в��
	qint32 g_totalVehicle = m_Vehicle.size();
	//��������в�ֲ���
	for(l_u16index = 0;l_u16index < l_FrameInfo1.u8Sum;l_u16index++)
	{
		l_32tmp = 0;  //ƥ���ʶ���ɹ���1��
		l_leftX = l_FrameInfo1.Ptdata[l_u16index].u16xLeft;
		l_rightX = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
		if (l_FrameInfo1.Ptdata[l_u16index].u16Rightpt <= l_FrameInfo1.Ptdata[l_u16index].u16Leftpt ||
			l_rightX <= l_leftX)
		{
			continue;
		}

		for(j = 0;j < g_totalVehicle;j++)
		{  	
			i = j;		
			if( (m_Vehicle[i]->u8Vstate == VehicleInfo::OCCURING_USED || m_Vehicle[i]->u8Istate != VehicleInfo::NO_USED) )
			{
				if(IS_INSIDE(l_leftX,l_rightX,m_Vehicle[i]->locateX.u16xLeft,m_Vehicle[i]->locateX.u16xRight))
				{
					l_32tmp = 1;  //����ƥ��ɹ�
					m_Vehicle[i]->u8Vstate = VehicleInfo::OCCURING_USED;	
					if(l_FrameInfo1.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //����	
						//						qDebug()<<"[sky2]match V";
						l_32tmp2 =m_Vehicle[i]->Vdata.u16FrameCnt &  FRAME_MASK;
						if(l_32tmp2)
						{
							m_Vehicle[i]->locateX.u16xLeft = min(l_leftX,m_Vehicle[i]->locateX.u16xLeft);
							m_Vehicle[i]->locateX.u16xRight = max(l_rightX,m_Vehicle[i]->locateX.u16xRight);
						}
						else
						{
							m_Vehicle[i]->locateX.u16xLeft = l_leftX;
							m_Vehicle[i]->locateX.u16xRight = l_rightX;
						}



						if(l_32tmp2 > 0 && Time_Vertical == m_Vehicle[i]->Vdata.tdata[l_32tmp2 - 1])
						{
							//ͬһ֡�У�������������ͬһ����
							m_Vehicle[i]->locateX.u16Leftpt = min(m_Vehicle[i]->locateX.u16Leftpt,l_FrameInfo1.Ptdata[l_u16index].u16Leftpt);

							m_Vehicle[i]->locateX.u16Rightpt = max(m_Vehicle[i]->locateX.u16Rightpt,l_FrameInfo1.Ptdata[l_u16index].u16Rightpt);
							l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;
							//memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
							const QList<qint32> & zBuffer = LMS_data_1.zdata();
							copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
							m_Vehicle[i]->Vdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;
							m_Vehicle[i]->Vdata.xdata[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xDis; 
						}
						else
						{
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;	
							if(m_Vehicle[i]->Vdata.u16FrameCnt < FRAME_MAXCNT )
							{
								l_leftPt  = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
								l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;
								const QList<qint32> & zBuffer = LMS_data_1.zdata();
								copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								//memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
								m_Vehicle[i]->Vdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
								m_Vehicle[i]->Vdata.xdata[l_32tmp2] = l_FrameInfo1.Ptdata[l_u16index].u16xDis; 
								m_Vehicle[i]->Vdata.zMax[l_32tmp2] = 	l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt; 						

								m_Vehicle[i]->Vdata.tdata[l_32tmp2] = Time_Vertical;
								m_Vehicle[i]->Vdata.u16FrameCnt++;
							}

						} 	
						m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);
						m_Vehicle[i]->VemptFrame = 0;										
					}
					else
					{
						//qDebug()<<"[sky1]no here";
						l_32tmp2 =m_Vehicle[i]->Vdata.u16FrameCnt &  FRAME_MASK;
						//���� 
						if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) > VEHICHLE_DISTANT_GAP && 
							abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) > VEHICHLE_DISTANT_GAP)
						{
							//����ָ��������ͬ������
							//ԭ������֡����   								
							l_leftPt  = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xLeft);
							l_rightPt = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xRight);   //��λ�ò�֧����͵��ڳ�����

							l_leftPt = max(l_leftPt,l_FrameInfo1.Ptdata[l_u16index].u16Leftpt);	
							l_rightPt = min(l_rightPt,l_FrameInfo1.Ptdata[l_u16index].u16Rightpt);

							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());

							if(l_leftPt < l_rightPt)
							{
								if(m_Vehicle[i]->Vdata.u16FrameCnt < FRAME_MAXCNT )
								{	  			
									const QList<qint32> & zBuffer = LMS_data_1.zdata();
									copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									//memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Vdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
									m_Vehicle[i]->Vdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft); 								

									m_Vehicle[i]->Vdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt); 

									m_Vehicle[i]->Vdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->locateX.u16Leftpt = l_leftPt;
									m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;
									m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);
									m_Vehicle[i]->Vdata.u16FrameCnt++;
								}	 

							}
							//							else
							//								break;
							m_Vehicle[i]->VemptFrame = 0;	

							//����һ���µ�����
							if((l_FrameInfo1.Ptdata[l_u16index].u16xRight - m_Vehicle[i]->locateX.u16xRight) > SMALL_AREA)
							{
								l_leftPt = 	m_Vehicle[i]->locateX.u16Rightpt + 1;
								l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

								correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());

								l_u16tmp = l_FrameInfo1.u8Sum++;
								l_FrameInfo1.Ptdata[l_u16tmp].u16xDis = abs(l_FrameInfo1.Ptdata[l_u16index].u16xRight - m_Vehicle[i]->locateX.u16xRight); 
								l_FrameInfo1.Ptdata[l_u16tmp].u16Leftpt = l_leftPt + 1;	 
								l_FrameInfo1.Ptdata[l_u16tmp].u16Rightpt = l_rightPt;  						
								l_FrameInfo1.Ptdata[l_u16tmp].u16xLeft  = l_FrameInfo1.Ptdata[l_u16index].u16xRight - l_FrameInfo1.Ptdata[l_u16tmp].u16xDis  + VEHICHLE_DISTANT_GAP;  
								l_FrameInfo1.Ptdata[l_u16tmp].u16xRight = l_FrameInfo1.Ptdata[l_u16index].u16xRight; 							
								l_FrameInfo1.Ptdata[l_u16tmp].u16xMaxHt	= GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt);	
							}

							//�޸�ԭ����
							l_leftPt = 	l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Leftpt -  1;

							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());

							l_FrameInfo1.Ptdata[l_u16index].u16Rightpt = l_rightPt;
							l_FrameInfo1.Ptdata[l_u16index].u16xRight = m_Vehicle[i]->locateX.u16xLeft - VEHICHLE_DISTANT_GAP;
							l_FrameInfo1.Ptdata[l_u16index].u16xDis = abs(l_FrameInfo1.Ptdata[l_u16index].u16xRight - l_FrameInfo1.Ptdata[l_u16index].u16xLeft);	
							if(l_FrameInfo1.Ptdata[l_u16index].u16xDis < SMALL_AREA)
							{
								l_u16index++;
							}
						}
						else if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) < abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX))
						{
							l_leftPt  = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xLeft);
							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());
							l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;   	
							if(l_leftPt < l_rightPt)
							{
								m_Vehicle[i]->locateX.u16Leftpt = l_leftPt;
								m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;
								m_Vehicle[i]->locateX.u16xRight = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
								m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);
								if(m_Vehicle[i]->Vdata.u16FrameCnt < FRAME_MAXCNT )
								{	  			
									const QList<qint32> & zBuffer = LMS_data_1.zdata();
									copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									//memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Vdata.zdata[l_32tmp2][0] = 	l_rightPt - l_leftPt;
									m_Vehicle[i]->Vdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft); 								

									m_Vehicle[i]->Vdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt);																							

									m_Vehicle[i]->Vdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->Vdata.u16FrameCnt++;
								}


							}
							m_Vehicle[i]->VemptFrame = 0;	
							//�������	  	
							l_FrameInfo1.Ptdata[l_u16index].u16xRight = m_Vehicle[i]->locateX.u16xLeft + VEHICHLE_DISTANT_GAP; 
							l_FrameInfo1.Ptdata[l_u16index].u16Rightpt = m_Vehicle[i]->locateX.u16Leftpt - 1;
							l_leftPt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());
							l_FrameInfo1.Ptdata[l_u16index].u16Leftpt = l_leftPt;
							l_FrameInfo1.Ptdata[l_u16index].u16Rightpt = l_rightPt;

							l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt);
							l_FrameInfo1.Ptdata[l_u16index].u16xDis -= m_Vehicle[i]->locateX.u16xDis;
							if(l_FrameInfo1.Ptdata[l_u16index].u16xDis < SMALL_AREA)
							{
								l_u16index++;
							}
						}
						else //if(abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) < VEHICHLE_DISTANT_GAP)
						{
							m_Vehicle[i]->locateX.u16xLeft = l_leftX;
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);

							l_leftPt  = m_Vehicle[i]->locateX.u16Leftpt;							
							l_rightPt = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xRight);  
							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());
							if(l_leftPt < l_rightPt)
							{
								m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;

								if(m_Vehicle[i]->Vdata.u16FrameCnt < FRAME_MAXCNT )
								{	  			
									const QList<qint32> & zBuffer = LMS_data_1.zdata();
									copy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									//memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Vdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
									m_Vehicle[i]->Vdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);								

									m_Vehicle[i]->Vdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt); 														
									m_Vehicle[i]->Vdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->Vdata.u16FrameCnt++;
								}

							}
							m_Vehicle[i]->VemptFrame = 0;	  

							//�������	  	
							l_FrameInfo1.Ptdata[l_u16index].u16xLeft = m_Vehicle[i]->locateX.u16xRight - VEHICHLE_DISTANT_GAP; 
							l_FrameInfo1.Ptdata[l_u16index].u16Leftpt = m_Vehicle[i]->locateX.u16Rightpt + 1;
							l_leftPt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

							correctPt(LMS_data_1.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterV());
							l_FrameInfo1.Ptdata[l_u16index].u16Leftpt = l_leftPt;
							l_FrameInfo1.Ptdata[l_u16index].u16Rightpt = l_rightPt;

							l_FrameInfo1.Ptdata[l_u16index].u16xMaxHt = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt);
							l_FrameInfo1.Ptdata[l_u16index].u16xDis -= m_Vehicle[i]->locateX.u16xDis;
							if(l_FrameInfo1.Ptdata[l_u16index].u16xDis < SMALL_AREA)
							{
								l_u16index++;
							}
						}

						l_u16index--;
					}
					break;
				}
			}

		}

		if(l_32tmp == 0)//u16InclineZeroPos wzf
		{ 
			//���³�          

			//�����Ƿ�Ϊ����
			if(l_FrameInfo1.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //���ڳ�����ȣ���Ϊ�ǲ���
			{
				qDebug()<<"[sky2]no here";
				l_leftX = l_FrameInfo1.Ptdata[l_u16index].u16xLeft;
				l_rightX = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
				l_leftPt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
				l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

				l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //��һ��λ�ô��ԭʼλ��
				l_u16PosVect[l_32tmp].u16Leftpt = l_leftPt;

				l_32tmp2 = l_leftPt + 1;
				TempVaule1 = l_leftX;
				while(l_32tmp2 < l_rightPt)
				{
					l_u16tmp = abs(LMS_data_1.xdata()[l_32tmp2] - TempVaule1);
					if(l_u16tmp > TrafficSettings::instance()->LaneWide() && LMS_data_1.xdata()[l_32tmp2])
					{
						l_u16PosVect[l_32tmp].u16xDis = l_u16tmp;
						l_u16PosVect[l_32tmp].u16xRight = LMS_data_1.xdata()[l_32tmp2] - VEHICHLE_DISTANT_GAP;

						correctPt(LMS_data_1.zdata(),&l_leftPt,&l_32tmp2,&TrafficSettings::instance()->getLasterV());
						if(l_32tmp2 <= l_leftPt)
						{
							if(l_leftPt+5 < l_rightPt)
								l_32tmp2 =l_leftPt + 5;
							else
								l_32tmp2 = l_rightPt;
						}

						l_u16PosVect[l_32tmp].u16Rightpt = l_32tmp2;
						l_u16PosVect[l_32tmp].u16xMaxHt = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_32tmp2);

						l_leftPt =  l_32tmp2;
						l_32tmp = (l_32tmp + 1) & POINTSET_MASK;
						l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //��һ��λ�ô��ԭʼλ��
						l_u16PosVect[l_32tmp].u16Leftpt = l_leftPt;
						TempVaule1 = LMS_data_1.xdata()[l_32tmp2];
					}
					l_32tmp2++;
				}

				while(l_leftPt < l_rightPt)
				{
					l_u16tmp = abs(l_rightX - LMS_data_1.xdata()[l_leftPt]);
					if (abs(LMS_data_1.xdata()[l_leftPt]) > FLY_LIMIT_VALUE)
					{
						if(l_u16tmp > SMALL_AREA)
						{
							l_u16PosVect[l_32tmp].u16xDis = l_u16tmp;
							l_u16PosVect[l_32tmp].u16xRight = l_rightX;
							l_u16PosVect[l_32tmp].u16Rightpt = l_rightPt;
							l_u16PosVect[l_32tmp].u16xMaxHt = GetMaxValue(LMS_data_1.zdata(),l_leftPt,l_rightPt);
							break;
						} //end if
						else
						{
							l_32tmp = l_32tmp > 0 ?l_32tmp-1:0;
							break;
						}


					}
					else
						l_leftPt++;
				}

			}
			else
			{
				memcpy(&l_u16PosVect[0],&l_FrameInfo1.Ptdata[l_u16index],sizeof(PointStruct));
				l_32tmp = 1;
			}

			for(l_u16tmp = 0;l_u16tmp<l_32tmp;l_u16tmp++)
			{
				VehicleInfo* t_newVeh = new VehicleInfo();
				m_Vehicle.append(t_newVeh);
				qDebug()<<"[V]new Vehicle detected";
				t_newVeh->u8Vstate = VehicleInfo::OCCURING_USED;
				t_newVeh->locateX.u16xLeft = l_u16PosVect[l_u16tmp].u16xLeft;
				t_newVeh->locateX.u16xRight = l_u16PosVect[l_u16tmp].u16xRight;
				t_newVeh->locateX.u16Leftpt = l_u16PosVect[l_u16tmp].u16Leftpt;
				t_newVeh->locateX.u16Rightpt = l_u16PosVect[l_u16tmp].u16Rightpt;
				t_newVeh->locateX.u16xDis = l_u16PosVect[l_u16tmp].u16xDis;

				l_leftPt = l_u16PosVect[l_u16tmp].u16Leftpt;
				l_rightPt = l_u16PosVect[l_u16tmp].u16Rightpt;
				l_leftX = l_u16PosVect[l_u16tmp].u16xLeft;
				l_rightX = l_u16PosVect[l_u16tmp].u16xRight;

				l_32tmp2 = t_newVeh->Vdata.u16FrameCnt = 0;

				const QList<qint32> & zBuffer = LMS_data_1.zdata();
				copy(&t_newVeh->Vdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
				//  memcpy(&m_Vehicle[i]->Vdata.zdata[l_32tmp2][1],LMS_data_1.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
				t_newVeh->Vdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
				t_newVeh->Vdata.xdata[l_32tmp2] = abs(l_rightX - l_leftX);
				t_newVeh->Vdata.zMax[l_32tmp2] = l_u16PosVect[l_u16tmp].u16xMaxHt;

				t_newVeh->Vdata.tdata[l_32tmp2] = Time_Vertical;
				t_newVeh->Vdata.u16FrameCnt++;                       

				t_newVeh->VemptFrame = 0;
			}

		} //end if���³� 
	}   //end for


	/**************************************************************************/
	/*************************��б����������***********************************/

	l_leftPt_absI = LMS_data_2.GetLeft();
	l_rightPt_absI = LMS_data_2.GetRight();
	//��������в�ֲ���
	for(l_u16index = 0;l_u16index < l_FrameInfo2.u8Sum ;l_u16index++)
	{
		l_32tmp = 0;  //ƥ���ʶ���ɹ���1��
		l_leftX = l_FrameInfo2.Ptdata[l_u16index].u16xLeft;
		l_rightX = l_FrameInfo2.Ptdata[l_u16index].u16xRight;
		if (l_FrameInfo2.Ptdata[l_u16index].u16Rightpt <= l_FrameInfo2.Ptdata[l_u16index].u16Leftpt ||
			l_rightX <= l_leftX)
		{
			continue;
		}
		for(j = 0;j < g_totalVehicle;j++)
		{  	
			i = j;//g_VehicleSetIndex[j] - 1;		
			if((m_Vehicle[i]->u8Vstate != VehicleInfo::NO_USED || m_Vehicle[i]->u8Istate == VehicleInfo::OCCURING_USED))
			{
				if(IS_INSIDE(l_leftX,l_rightX,m_Vehicle[i]->locateX.u16xLeft,m_Vehicle[i]->locateX.u16xRight))
				{
					l_32tmp = 1;  //����ƥ��ɹ�
					m_Vehicle[i]->u8Istate = VehicleInfo::OCCURING_USED;	
					if(l_FrameInfo2.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //����								
						//					   qDebug()<<"[sky3]match Q";
						m_Vehicle[i]->locateX.u16xLeft = min(l_leftX,m_Vehicle[i]->locateX.u16xLeft);	 
						m_Vehicle[i]->locateX.u16xRight = max(l_rightX,m_Vehicle[i]->locateX.u16xRight); 						  				

						m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xLeft -  m_Vehicle[i]->locateX.u16xRight);	

						l_32tmp2 =m_Vehicle[i]->Incdata.u16FrameCnt &  FRAME_MASK;
						if(l_32tmp2 > 0 && Time_Vertical == m_Vehicle[i]->Incdata.tdata[l_32tmp2 - 1])
						{
							//ͬһ֡�У�������������ͬһ����
							m_Vehicle[i]->locateX.u16Leftpt = min(m_Vehicle[i]->locateX.u16Leftpt,l_FrameInfo2.Ptdata[l_u16index].u16Leftpt);

							m_Vehicle[i]->locateX.u16Rightpt = max(m_Vehicle[i]->locateX.u16Rightpt,l_FrameInfo2.Ptdata[l_u16index].u16Rightpt);
							l_leftPt = m_Vehicle[i]->locateX.u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Rightpt;
							const QList<qint32> & zBuffer = LMS_data_2.zdata();
							copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
							//memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
							m_Vehicle[i]->Incdata.zdata[l_32tmp2][0]  = l_rightPt - l_leftPt;
							m_Vehicle[i]->Incdata.xdata[l_32tmp2] += l_FrameInfo2.Ptdata[l_u16index].u16xDis; 
							//						   m_Vehicle[i]->Incdata.ydata[l_32tmp2] = GetMaxValue(g_YdistanceI,l_leftPt,l_rightPt);
						}
						else
						{
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							m_Vehicle[i]->locateX.u16Rightpt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;	
							if(m_Vehicle[i]->Incdata.u16FrameCnt < FRAME_MAXCNT )
							{
								l_leftPt  = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
								l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;
								const QList<qint32> & zBuffer = LMS_data_2.zdata();
								copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
								//memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
								m_Vehicle[i]->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
								m_Vehicle[i]->Incdata.xdata[l_32tmp2] = l_FrameInfo2.Ptdata[l_u16index].u16xDis; 
								m_Vehicle[i]->Incdata.zMax[l_32tmp2] = 	l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt;
								///							   m_Vehicle[i]->Incdata.ydata[l_32tmp2] = GetMaxValue(g_YdistanceI,l_leftPt,l_rightPt); 						
								m_Vehicle[i]->Incdata.tdata[l_32tmp2] = Time_Vertical;
								m_Vehicle[i]->Incdata.u16FrameCnt++;
							}

						} 


						m_Vehicle[i]->IemptFrame = 0;										
					}
					else
					{
						qDebug()<<"[sky3]no here";
						l_32tmp2 =m_Vehicle[i]->Incdata.u16FrameCnt &  FRAME_MASK;
						//���� 
						if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) > VEHICHLE_DISTANT_GAP && 
							abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) > VEHICHLE_DISTANT_GAP)
						{
							//����ָ��������ͬ������
							//ԭ������֡����   								
							l_leftPt  = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xLeft);
							l_rightPt = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xRight);   //��λ�ò�֧����͵��ڳ�����

							l_leftPt = max(l_leftPt,l_FrameInfo2.Ptdata[l_u16index].u16Leftpt);	 
							l_rightPt = min(l_rightPt,l_FrameInfo2.Ptdata[l_u16index].u16Rightpt);  
							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());

							if(l_leftPt < l_rightPt)
							{
								if(m_Vehicle[i]->Incdata.u16FrameCnt < FRAME_MAXCNT )
								{
									const QList<qint32> & zBuffer = LMS_data_2.zdata();
									copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									// memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
									m_Vehicle[i]->Incdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft);

									m_Vehicle[i]->Incdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);
									//                                   m_Vehicle[i]->Incdata.ydata[l_32tmp2] = GetMaxValue(g_YdistanceI,l_leftPt,l_rightPt);


									m_Vehicle[i]->Incdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->locateX.u16Leftpt = l_leftPt;
									m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;
									m_Vehicle[i]->Incdata.u16FrameCnt++;
								}

							}
							//						   else
							//								break;

							m_Vehicle[i]->IemptFrame = 0;	

							//����һ���µ�����
							if((l_FrameInfo2.Ptdata[l_u16index].u16xRight - m_Vehicle[i]->locateX.u16xRight) > SMALL_AREA)
							{
								l_leftPt = 	m_Vehicle[i]->locateX.u16Rightpt;
								l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;
								correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());
								l_u16tmp = l_FrameInfo2.u8Sum++;
								l_FrameInfo2.Ptdata[l_u16tmp].u16xDis = abs(l_FrameInfo2.Ptdata[l_u16index].u16xRight - m_Vehicle[i]->locateX.u16xRight); 
								l_FrameInfo2.Ptdata[l_u16tmp].u16Leftpt = l_leftPt + 1;	 
								l_FrameInfo2.Ptdata[l_u16tmp].u16Rightpt = l_rightPt;  						
								l_FrameInfo2.Ptdata[l_u16tmp].u16xLeft  = l_FrameInfo2.Ptdata[l_u16index].u16xRight - l_FrameInfo2.Ptdata[l_u16tmp].u16xDis + VEHICHLE_DISTANT_GAP;  
								l_FrameInfo2.Ptdata[l_u16tmp].u16xRight = l_FrameInfo2.Ptdata[l_u16index].u16xRight; 							
								l_FrameInfo2.Ptdata[l_u16tmp].u16xMaxHt	= GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);	
							}

							//�޸�ԭ����
							l_leftPt = 	l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = m_Vehicle[i]->locateX.u16Leftpt -  1;

							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());

							l_FrameInfo2.Ptdata[l_u16index].u16Rightpt = l_rightPt;
							l_FrameInfo2.Ptdata[l_u16index].u16xRight = m_Vehicle[i]->locateX.u16xLeft - VEHICHLE_DISTANT_GAP;
							l_FrameInfo2.Ptdata[l_u16index].u16xDis = abs(l_FrameInfo2.Ptdata[l_u16index].u16xRight - l_FrameInfo2.Ptdata[l_u16index].u16xLeft);	
							if(l_FrameInfo2.Ptdata[l_u16index].u16xDis < SMALL_AREA)
							{
								l_u16index++;							  
							}
						}
						else if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) < abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) )
						{
							l_leftPt  = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xLeft);
							l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;   		//��λ�ò�֧����͵��ڳ�����
							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());
							if(l_leftPt < l_rightPt)
							{
								m_Vehicle[i]->locateX.u16Leftpt = l_leftPt;
								m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;
								m_Vehicle[i]->locateX.u16xRight = l_FrameInfo2.Ptdata[l_u16index].u16xRight;
								m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xLeft -  m_Vehicle[i]->locateX.u16xRight);
								if(m_Vehicle[i]->Incdata.u16FrameCnt < FRAME_MAXCNT )
								{	  			
									const QList<qint32> & zBuffer = LMS_data_2.zdata();
									copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									// memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
									m_Vehicle[i]->Incdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft); 								

									m_Vehicle[i]->Incdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);	
									//								   m_Vehicle[i]->Incdata.ydata[l_32tmp2] = GetMaxValue(g_YdistanceI,l_leftPt,l_rightPt);																						

									m_Vehicle[i]->Incdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->Incdata.u16FrameCnt++;
								}

							}

							m_Vehicle[i]->IemptFrame = 0;
							//�������
							l_FrameInfo2.Ptdata[l_u16index].u16xRight = m_Vehicle[i]->locateX.u16xLeft + VEHICHLE_DISTANT_GAP;
							l_FrameInfo2.Ptdata[l_u16index].u16Rightpt = m_Vehicle[i]->locateX.u16Leftpt - 1;


							l_leftPt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;

							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());
							l_FrameInfo2.Ptdata[l_u16index].u16Leftpt = l_leftPt;
							l_FrameInfo2.Ptdata[l_u16index].u16Rightpt = l_rightPt;

							l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);	
							l_FrameInfo2.Ptdata[l_u16index].u16xDis -= m_Vehicle[i]->locateX.u16xDis;
							if(l_FrameInfo2.Ptdata[l_u16index].u16xDis < SMALL_AREA)
							{
								l_u16index++;
							}
						}
						else 
						{
							m_Vehicle[i]->locateX.u16xLeft = l_leftX;
							m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xLeft -  m_Vehicle[i]->locateX.u16xRight);
							m_Vehicle[i]->locateX.u16Leftpt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							l_leftPt  = m_Vehicle[i]->locateX.u16Leftpt;

							l_rightPt = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xRight);   
							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());
							if(l_leftPt < l_rightPt)
							{
								m_Vehicle[i]->locateX.u16Rightpt = l_rightPt;

								if(m_Vehicle[i]->Incdata.u16FrameCnt < FRAME_MAXCNT )
								{	  			
									const QList<qint32> & zBuffer = LMS_data_2.zdata();
									copy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
									//memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
									m_Vehicle[i]->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
									m_Vehicle[i]->Incdata.xdata[l_32tmp2] = abs(m_Vehicle[i]->locateX.u16xRight - m_Vehicle[i]->locateX.u16xLeft); 								

									m_Vehicle[i]->Incdata.zMax[l_32tmp2] = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt); 
									//								   m_Vehicle[i]->Incdata.ydata[l_32tmp2] = GetMaxValue(g_YdistanceI,l_leftPt,l_rightPt);														

									m_Vehicle[i]->Incdata.tdata[l_32tmp2] = Time_Vertical;
									m_Vehicle[i]->Incdata.u16FrameCnt++;
								}

							}

							m_Vehicle[i]->IemptFrame = 0;

							//�������
							l_FrameInfo2.Ptdata[l_u16index].u16xLeft = m_Vehicle[i]->locateX.u16xRight - VEHICHLE_DISTANT_GAP;
							l_FrameInfo2.Ptdata[l_u16index].u16Leftpt = m_Vehicle[i]->locateX.u16Rightpt + 1;
							l_leftPt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
							l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;

							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_rightPt,&TrafficSettings::instance()->getLasterQ());
							l_FrameInfo2.Ptdata[l_u16index].u16Leftpt = l_leftPt;
							l_FrameInfo2.Ptdata[l_u16index].u16Rightpt = l_rightPt;

							l_FrameInfo2.Ptdata[l_u16index].u16xMaxHt = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);
							l_FrameInfo2.Ptdata[l_u16index].u16xDis -= m_Vehicle[i]->locateX.u16xDis;
							if(l_FrameInfo2.Ptdata[l_u16index].u16xDis < 1000)
							{
								l_u16index++;
							}
						}

						l_u16index--;
					}
					break;
				}
			}

		}


		if(l_32tmp == 0)//u16InclineZeroPos wzf
		{ //���³�
			//			l_u16tmp = (l_FrameInfo2.Ptdata[l_u16index].u16Leftpt + l_FrameInfo2.Ptdata[l_u16index].u16Rightpt)>>1;
			//			if(l_u16tmp > l_u16PtCenter)
			{

				//�����Ƿ�Ϊ���� 
				if(l_FrameInfo2.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //���ڳ�����ȣ���Ϊ�ǲ���
				{ 	
					qDebug()<<"[sky4]no here";
					l_leftX = l_FrameInfo2.Ptdata[l_u16index].u16xLeft;
					l_rightX = l_FrameInfo2.Ptdata[l_u16index].u16xRight;	
					l_leftPt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
					l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;		
					l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //��һ��λ�ô��ԭʼλ��	
					l_u16PosVect[l_32tmp].u16Leftpt = l_leftPt;
					l_32tmp2 = l_leftPt + 1;
					TempVaule1 = l_leftX;
					while(l_32tmp2 < l_rightPt)
					{ 		   
						l_u16tmp = abs(LMS_data_2.xdata()[l_32tmp2] - TempVaule1); 	
						if(l_u16tmp > TrafficSettings::instance()->LaneWide() && LMS_data_2.xdata()[l_32tmp2])
						{
							l_u16PosVect[l_32tmp].u16xDis = l_u16tmp;
							l_u16PosVect[l_32tmp].u16xRight = LMS_data_2.xdata()[l_32tmp2] - VEHICHLE_DISTANT_GAP;

							correctPt(LMS_data_2.zdata(),&l_leftPt,&l_32tmp2,&TrafficSettings::instance()->getLasterQ());
							if(l_32tmp2 <= l_leftPt)
							{
								if(l_leftPt+5 < l_rightPt)
									l_32tmp2 =l_leftPt + 5;	
								else
									l_32tmp2 = l_rightPt;	
							}

							l_u16PosVect[l_32tmp].u16Rightpt = l_32tmp2;
							l_u16PosVect[l_32tmp].u16xMaxHt = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_32tmp2);	  

							l_leftPt =  l_32tmp2;
							l_32tmp = (l_32tmp + 1) & POINTSET_MASK;
							l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //��һ��λ�ô��ԭʼλ��	
							l_u16PosVect[l_32tmp].u16Leftpt = l_leftPt;
							TempVaule1 = LMS_data_1.xdata()[l_32tmp2];
						}  
						l_32tmp2++;	
					}

					while(l_leftPt < l_rightPt)
					{
						l_u16tmp = abs(l_rightX - LMS_data_2.xdata()[l_leftPt]);
						if (abs(LMS_data_2.xdata()[l_leftPt]) > FLY_LIMIT_VALUE)
						{
							if(l_u16tmp > SMALL_AREA)
							{
								l_u16PosVect[l_32tmp].u16xDis = l_u16tmp;
								l_u16PosVect[l_32tmp].u16xRight = l_rightX;
								l_u16PosVect[l_32tmp].u16Rightpt = l_rightPt;
								l_u16PosVect[l_32tmp].u16xMaxHt = GetMaxValue(LMS_data_2.zdata(),l_leftPt,l_rightPt);
								break;
							} //end if
							else
							{
								l_32tmp = l_32tmp > 0 ?l_32tmp-1:0;
								break;
							}


						}
						else
							l_leftPt++;
					}

				}
				else
				{
					memcpy(&l_u16PosVect[0],&l_FrameInfo2.Ptdata[l_u16index],sizeof(PointStruct));
					l_32tmp = 1;
				}

				for(l_u16tmp = 0;l_u16tmp<l_32tmp;l_u16tmp++)
				{
					VehicleInfo* t_newVeh = new VehicleInfo();
					m_Vehicle.append(t_newVeh);
					qDebug()<<"[Q]new Vehicle detected";
					t_newVeh->u8Vstate = VehicleInfo::OCCURING_USED;
					t_newVeh->locateX.u16xLeft = l_u16PosVect[l_u16tmp].u16xLeft;
					t_newVeh->locateX.u16xRight = l_u16PosVect[l_u16tmp].u16xRight;
					t_newVeh->locateX.u16Leftpt = l_u16PosVect[l_u16tmp].u16Leftpt;
					t_newVeh->locateX.u16Rightpt = l_u16PosVect[l_u16tmp].u16Rightpt;
					t_newVeh->locateX.u16xDis = l_u16PosVect[l_u16tmp].u16xDis;

					l_leftPt = l_u16PosVect[l_u16tmp].u16Leftpt;
					l_rightPt = l_u16PosVect[l_u16tmp].u16Rightpt;
					l_leftX = l_u16PosVect[l_u16tmp].u16xLeft;
					l_rightX = l_u16PosVect[l_u16tmp].u16xRight;

					l_32tmp2 = t_newVeh->Incdata.u16FrameCnt = 0;

					const QList<qint32> & zBuffer = LMS_data_2.zdata();
					copy(&t_newVeh->Incdata.zdata[l_32tmp2][1],zBuffer.begin() + l_leftPt,zBuffer.begin() + l_rightPt);
					//memcpy(&m_Vehicle[i]->Incdata.zdata[l_32tmp2][1],LMS_data_2.zdata() + l_leftPt,sizeof(int32)*(l_rightPt - l_leftPt));
					t_newVeh->Incdata.zdata[l_32tmp2][0] = l_rightPt - l_leftPt;
					t_newVeh->Incdata.xdata[l_32tmp2] = abs(l_rightX - l_leftX);
					t_newVeh->Incdata.zMax[l_32tmp2] = l_u16PosVect[l_u16tmp].u16xMaxHt;

					t_newVeh->Incdata.tdata[l_32tmp2] = Time_Vertical;
					t_newVeh->Incdata.u16FrameCnt++;
					t_newVeh->IemptFrame = 0;

				}
			}
		} //end if���³�

	}   //end for

	QListIterator<VehicleInfo*> iEveryCompleteItem(m_Vehicle);
	while(iEveryCompleteItem.hasNext())
	{
		VehicleInfo* const & ref_VehItem = iEveryCompleteItem.next();

		if(ref_VehItem->u8Vstate != VehicleInfo::NO_USED || ref_VehItem->u8Istate != VehicleInfo::NO_USED)
		{
			if(ref_VehItem->VemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Vdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Vstate = VehicleInfo::PASSED_USED;  //�ѽ���������β�ĳ�
			}
			if(ref_VehItem->IemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Incdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Istate = VehicleInfo::PASSED_USED;  //�ѽ���������β�ĳ�
			}

			if(ref_VehItem->IemptFrame >ERR_MAX_EMPTYFRAME && ref_VehItem->VemptFrame >ERR_MAX_EMPTYFRAME)
			{
				delete ref_VehItem;
				m_Vehicle.removeOne(ref_VehItem);    
				qDebug()<<"Vehicle ignored";
				continue;
			}
			else if(ref_VehItem->u8Istate == VehicleInfo::PASSED_USED && ref_VehItem->u8Vstate == VehicleInfo::PASSED_USED)
			{
				//��β����
				qDebug()<<"Vehicle accepted";
				//VehModels2(ref_VehItem);
				delete ref_VehItem;
				m_Vehicle.removeOne(ref_VehItem);   
				continue;

			}

			if(ref_VehItem->locateX.u16xDis > 2800)
			{
				int32 t_midX = (ref_VehItem->locateX.u16xLeft + ref_VehItem->locateX.u16xRight)>>1;
				ref_VehItem->locateX.u16xLeft = t_midX - 1100;
				ref_VehItem->locateX.u16xRight  = t_midX + 1100;
				ref_VehItem->locateX.u16xDis = 2200;
			}

			ref_VehItem->VemptFrame++;
			ref_VehItem->IemptFrame++;
		}


	}

}

#endif

#endif