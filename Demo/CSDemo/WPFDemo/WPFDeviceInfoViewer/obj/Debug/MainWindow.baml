   M S B A M L   `   `   `  ����  WPFDeviceInfoViewer$    WPFDeviceInfoViewer.MainWindow   5   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	     � Title$  
MainWindow��5   	   $	��350��6   $	��800��6)    � WindowStartupLocation$ CenterScreen=�65    � 	Resources 5         XmlLogProvider � XPath��%2   &	        � 5   
   $ Sources/Source��61   5      .��5      � .+�5	   
   R� $��0��6   $��0��6#   ��5
      �� 5       �� Source# [  6   ��5      �� 5       �� ItemsSource 5      �� 5       �� XPath$ child::node()��6"   .R�5      ��  �� Orientation$ 
Horizontal=�6!   .+�5      ��  ��Name$ mtitleTextblock��6$   �� 5      	 ��Name$	 mValuesPanel��6%   $y�	Collapsed=�6;   $ 
Horizontal=�6R   .+�5      �� $��: ��6(   �� 5      $ mvalueTextblock��6<   $��
5, 0, 0, 0q�6(   6<   �� 5      $	 mSecondValuesPanel��6=   $��
0, 0, 0, 0q�6)   $y�	Collapsed=�6Y   $ 
Horizontal=�6p   .+�5   "   �� $ msecondtitleTextblock��6,   �� 5   "   $��: ��6,   �� 5   "   $ msecondvalueTextblock��6@   $��
5, 0, 0, 0q�6,   6@   5      5      5      
 �� Triggers
 5      �� 5        �� Value$ Source��6D    �� Binding 6&   ��  �� Path$
 Name�.a�5!      ��  �� 
TargetName$ mtitleTextblock��6%    �� Property � �� �� Value 5"   "   �� 5#   &   $b \Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME']/SingleValue/@Value��6.   5%      �� 5&      $ mtitleTextblock��6%    �@�ToolTip 5'   "   �� 5(   &   $h bSource.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME']/SingleValue/@Description��6.   5*      5+      �� 5,      $ Source.Attributes��6D    6&   �� $
 Name�.a�5-      �� $ mtitleTextblock��6%    � ��$ Attributes of source��6R   5.      �� 5/      $ 	Attribute��6D    6&   �� $
 Name�.a�50      �� $ mtitleTextblock��6%    � �� 51   "   �� 52   &   $ @Title��6.   54      �� 55      $ mtitleTextblock��6%    �@�ToolTip 56   "   �� 57   &   $ @Description��6.   59      5:      �� 5;      $ DeviceGroup��6D    6&   �� $
 Name�.a�5<      �� $ mtitleTextblock��6%    � �� 5=   "   �� 5>   &   $ @Title��6.   5@      �� 5A      $ mtitleTextblock��6%    �@�ToolTip 5B   "   �� 5C   &   $ @Description��6.   5E      5F      �� 5I      $ SingleValue��6D    6&   �� $
 Name�.a�5J      �� $ mtitleTextblock��6%    � �� 5K   "   �� 5L   &   $ @Value��6.   5N      5O      �� 5P      $ Value.ValueParts��6D    6&   �� $
 Name�.a�5Q      �� $ mtitleTextblock��6%    � ��$ Values��6R   5R      �� 5S      $ 	ValuePart��6D    6&   �� $
 Name�.a�5T      �� $ mtitleTextblock��6%    � �� 5U   "   �� 5V   &   $ @Title��6.   5X      �� 5Y      $ mvalueTextblock��6%    � �� 5Z   "   �� 5[   &   $ @Value��6.   5]      �� 5^      $ mValuesPanel��6%    � y�$ Visible=�6U   5_      �� 5`      $ PresentationDescriptor��6D    6&   �� $
 Name�.a�5a      �� $ mtitleTextblock��6%    � ��$! Presentation - Stream count��6R   �� 5b      $ mvalueTextblock��6%    � �� 5c   "   �� 5d   &   $ @StreamCount��6.   5f      �� 5g      $ mValuesPanel��6%    � y�$ Visible=�6U   5h      �� 5i      $ StreamDescriptor��6D    6&   �� $
 Name�.a�5j      �� $ mtitleTextblock��6%    � ��$ Stream - Index��6R   �� 5k      $ mvalueTextblock��6%    � �� 5l   "   �� 5m   &   $ @Index��6.   5o      �� 5p      $ mValuesPanel��6%    � y�$ Visible=�6U   �� 5q      $ mSecondValuesPanel��6%    � y�$ Visible=�6[   �� 5r      $ msecondtitleTextblock��6%    � ��$ ; StreamType��6X   �� 5s      $ msecondvalueTextblock��6%    � �� 5t   "   �� 5u   &   $ 
@MajorType��6.   5w      5x      �� 5y      $ 
MediaTypes��6D    6&   �� $
 Name�.a�5z      �� $ mtitleTextblock��6%    � ��$# MediaTypes - MediaTypes count��6R   �� 5{      $ mvalueTextblock��6%    � �� 5|   "   �� 5}   &   $ 
@TypeCount��6.   5      �� 5�      $ mValuesPanel��6%    � y�$ Visible=�6U   5�      �� 5�      $ 	MediaType��6D    6&   �� $
 Name�.a�5�      �� $ mtitleTextblock��6%    � ��$ MediaType - Index��6R   �� 5�      $ mvalueTextblock��6%    � �� 5�   "   �� 5�   &   $ @Index��6.   5�      �� 5�      $ mValuesPanel��6%    � y�$ Visible=�6U   5�      �� 5�      $ MediaTypeItem��6D    6&   �� $
 Name�.a�5�      �� $ mtitleTextblock��6%    � �� 5�   "   �� 5�   &   $ @Title��6.   5�      �� 5�      $ mtitleTextblock��6%    �@�ToolTip 5�   "   �� 5�   &   $ @Description��6.   5�      5�      �� 5�      $ 
RatioValue��6D    6&   �� $
 Name�.a�5�      �� $ mtitleTextblock��6%    � �� 5�   "   �� 5�   &   $ @Value��6.   5�      5�      �� 5�      $' !PresentationDescriptor.Attributes��6D    6&   �� $
 Name�.a�5�      �� $ mtitleTextblock��6%    � �� 5�   "   �� 5�   &   $ @Title��6.   5�      5�      �� 5�      $! StreamDescriptor.Attributes��6D    6&   �� $
 Name�.a�5�      �� $ mtitleTextblock��6%    � �� 5�   "   �� 5�   &   $ @Title��6.   5�      5�      5�      5�      5�      5�      