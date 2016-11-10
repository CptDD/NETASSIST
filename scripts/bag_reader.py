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
parser.add_argument('-b','--bag',required=True,help='bag to be processed')
args=vars(parser.parse_args())

bag=rosbag.Bag(args['bag'],'r')

print('We are here to make some noise!')

cv_bridge=CvBridge()

frame_writer=cv2.VideoWriter('result.avi',cv2.cv.CV_FOURCC(*'H264'),20,(640,480))


for topic, msg, t in bag.read_messages(topics=['images']):
	image=cv_bridge.imgmsg_to_cv2(msg,'bgr8')
	cv2.imshow('Frame',image)
	cv2.waitKey(33)
	frame_writer.write(image)

