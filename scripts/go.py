#!/usr/bin/env python

from __future__ import print_function
import rospy
from imutils.object_detection import non_max_suppression
from imutils import paths
import numpy as np
import argparse
import cv2
import imutils
import datetime
import rosbag
import sys
from std_msgs.msg import String
from sensor_msgs.msg import Image
from cv_bridge import CvBridge, CvBridgeError



parser=argparse.ArgumentParser()
parser.add_argument('-v','--video',required=True,help='path to the video file')
args=vars(parser.parse_args())


bag=rosbag.Bag('video.bag','w')

cap=cv2.VideoCapture(args['video'])

bridge=CvBridge()


while cap.isOpened():

	success,frame=cap.read()
	cv2.imshow('Frame',frame)

	frame=imutils.resize(frame,width=min(480,frame.shape[1]))

	cv_image=bridge.cv2_to_imgmsg(frame,'bgr8')

	bag.write('/images',cv_image)

	if cv2.waitKey(33) & 0xFF==ord('q'):
		break