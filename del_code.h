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
	PointStruct l_u16PosVect[POINTSET_CNT];	//存放并车的位置信息
	uint16 l_u16index,l_u16tmp;

	uint16 l_leftPt = 0,l_rightPt = 0;	//左,右点数
	int32 l_leftX = 0,l_rightX = 0;    //左，右x距离
	uint16 l_leftPt_absV = 0,l_rightPt_absV = 0;
	uint16 l_leftPt_absI = 0,l_rightPt_absI = 0;

	PointSet l_FrameInfo1,l_FrameInfo2;
	//帧数据进行分有车区域,记录每个区域的左右位置信息
	LMS_data_1.splitArea(TrafficSettings::instance()->getLasterV(),m_vBackground,&l_FrameInfo1);

	LMS_data_2.splitArea(TrafficSettings::instance()->getLasterQ(),m_qBackground,&l_FrameInfo2);

	//计算车辆的车道号时，车道计算的起始位置与上面计算的位置存在隔离区偏差
	//故而放在此处计算最为合适，不常变化，仅计算一次就OK。
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


	//上述区域进行拆分的过程中包括有几种情况：
	//1、打非点存在的情况下，计算的区域宽度较大
	//2、并车行驶时，区域连成一边，需要进行拆分
	qint32 g_totalVehicle = m_Vehicle.size();
	//对区域进行拆分操作
	for(l_u16index = 0;l_u16index < l_FrameInfo1.u8Sum;l_u16index++)
	{
		l_32tmp = 0;  //匹配标识，成功置1。
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
					l_32tmp = 1;  //区域匹配成功
					m_Vehicle[i]->u8Vstate = VehicleInfo::OCCURING_USED;	
					if(l_FrameInfo1.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //单车	
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
							//同一帧中，有两个区域是同一车的
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
						//并车 
						if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) > VEHICHLE_DISTANT_GAP && 
							abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) > VEHICHLE_DISTANT_GAP)
						{
							//区域分割成两个不同的区域
							//原来车辆帧数据   								
							l_leftPt  = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xLeft);
							l_rightPt = GetPosFromXDistance(LMS_data_1.xdata(),l_leftPt_absV,l_rightPt_absV,m_Vehicle[i]->locateX.u16xRight);   //此位置不支持最低点在车道上

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

							//加入一个新的区域
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

							//修改原区域
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
							//区域更新	  	
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

							//区域更新	  	
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
			//进新车          

			//区域是否为并车
			if(l_FrameInfo1.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //大于车道宽度，认为是并车
			{
				qDebug()<<"[sky2]no here";
				l_leftX = l_FrameInfo1.Ptdata[l_u16index].u16xLeft;
				l_rightX = l_FrameInfo1.Ptdata[l_u16index].u16xRight;
				l_leftPt = l_FrameInfo1.Ptdata[l_u16index].u16Leftpt;
				l_rightPt = l_FrameInfo1.Ptdata[l_u16index].u16Rightpt;

				l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //第一个位置存放原始位置
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
						l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //第一个位置存放原始位置
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

		} //end if进新车 
	}   //end for


	/**************************************************************************/
	/*************************倾斜激光器处理***********************************/

	l_leftPt_absI = LMS_data_2.GetLeft();
	l_rightPt_absI = LMS_data_2.GetRight();
	//对区域进行拆分操作
	for(l_u16index = 0;l_u16index < l_FrameInfo2.u8Sum ;l_u16index++)
	{
		l_32tmp = 0;  //匹配标识，成功置1。
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
					l_32tmp = 1;  //区域匹配成功
					m_Vehicle[i]->u8Istate = VehicleInfo::OCCURING_USED;	
					if(l_FrameInfo2.Ptdata[l_u16index].u16xDis <= TrafficSettings::instance()->LaneWide())
					{  //单车								
						//					   qDebug()<<"[sky3]match Q";
						m_Vehicle[i]->locateX.u16xLeft = min(l_leftX,m_Vehicle[i]->locateX.u16xLeft);	 
						m_Vehicle[i]->locateX.u16xRight = max(l_rightX,m_Vehicle[i]->locateX.u16xRight); 						  				

						m_Vehicle[i]->locateX.u16xDis = abs(m_Vehicle[i]->locateX.u16xLeft -  m_Vehicle[i]->locateX.u16xRight);	

						l_32tmp2 =m_Vehicle[i]->Incdata.u16FrameCnt &  FRAME_MASK;
						if(l_32tmp2 > 0 && Time_Vertical == m_Vehicle[i]->Incdata.tdata[l_32tmp2 - 1])
						{
							//同一帧中，有两个区域是同一车的
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
						//并车 
						if(abs(m_Vehicle[i]->locateX.u16xRight - l_rightX) > VEHICHLE_DISTANT_GAP && 
							abs(m_Vehicle[i]->locateX.u16xLeft - l_leftX) > VEHICHLE_DISTANT_GAP)
						{
							//区域分割成两个不同的区域
							//原来车辆帧数据   								
							l_leftPt  = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xLeft);
							l_rightPt = GetPosFromXDistance(LMS_data_2.xdata(),l_leftPt_absI,l_rightPt_absI,m_Vehicle[i]->locateX.u16xRight);   //此位置不支持最低点在车道上

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

							//加入一个新的区域
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

							//修改原区域
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
							l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;   		//此位置不支持最低点在车道上
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
							//区域更新
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

							//区域更新
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
		{ //进新车
			//			l_u16tmp = (l_FrameInfo2.Ptdata[l_u16index].u16Leftpt + l_FrameInfo2.Ptdata[l_u16index].u16Rightpt)>>1;
			//			if(l_u16tmp > l_u16PtCenter)
			{

				//区域是否为并车 
				if(l_FrameInfo2.Ptdata[l_u16index].u16xDis > TrafficSettings::instance()->LaneWide())  //大于车道宽度，认为是并车
				{ 	
					qDebug()<<"[sky4]no here";
					l_leftX = l_FrameInfo2.Ptdata[l_u16index].u16xLeft;
					l_rightX = l_FrameInfo2.Ptdata[l_u16index].u16xRight;	
					l_leftPt = l_FrameInfo2.Ptdata[l_u16index].u16Leftpt;
					l_rightPt = l_FrameInfo2.Ptdata[l_u16index].u16Rightpt;		
					l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //第一个位置存放原始位置	
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
							l_u16PosVect[l_32tmp].u16xLeft = l_leftX; //第一个位置存放原始位置	
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
		} //end if进新车

	}   //end for

	QListIterator<VehicleInfo*> iEveryCompleteItem(m_Vehicle);
	while(iEveryCompleteItem.hasNext())
	{
		VehicleInfo* const & ref_VehItem = iEveryCompleteItem.next();

		if(ref_VehItem->u8Vstate != VehicleInfo::NO_USED || ref_VehItem->u8Istate != VehicleInfo::NO_USED)
		{
			if(ref_VehItem->VemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Vdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Vstate = VehicleInfo::PASSED_USED;  //已结束，可收尾的车
			}
			if(ref_VehItem->IemptFrame > NORMAL_MAX_EMPTYFRAME && ref_VehItem->Incdata.u16FrameCnt > 1)
			{
				ref_VehItem->u8Istate = VehicleInfo::PASSED_USED;  //已结束，可收尾的车
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
				//收尾处理。
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