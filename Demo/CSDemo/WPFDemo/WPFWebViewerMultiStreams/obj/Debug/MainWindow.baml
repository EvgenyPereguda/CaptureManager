   M S B A M L   `   `   `  ����[  WSystem.Windows.Forms, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089Y@clr-namespace:System.Windows.Forms;assembly=System.Windows.FormsSystem.Windows.Forms  5       WPFWebViewerMultiStreams)   #WPFWebViewerMultiStreams.MainWindow   5   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35^ ZWindowsFormsIntegration, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35F 9http://schemas.microsoft.com/winfx/2006/xaml/presentation     5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	   Owf@clr-namespace:System.Windows.Forms;assembly=System.Windows.Forms     5      -   5          � Title$  
MainWindow��5   	   $	��500��6   $	��800��6)    � WindowStartupLocation$ CenterScreen=�5   "    � 	Resources 5         XmlLogProvider � XPath  XmlWebCamParametrsProvider��%O   &	        &	 (     � 5   
   $ Sources/Source��61   � 5	   
   6   5
      .��5      �  � RowDefinitions 5   
   .+�5   
   � $��0��6   $��0��6    � ColumnDefinitions 5      �� 5      $
��395* �6#   �� 5      $
��395* �6#    5      �� 5      $��45 �6    �� 5      $��45 �6    �� 5      .+�5      �� -   5         ��Name$ mLaunchButton��5   1   $��Launch��6H   $��28(�6Y   $��0��6g   $��0��6t   $��127,0,125,5q�6�   �� 5      -   5        $ mSecondLaunchButton��5   1   $��Launch��6N   $��28(�6_   $��0��6m   $��1��6z   $��
122,0,98,5q�6�   �� 5      -   5        $��1��5       � Value$	 100Q�6E    � Maximum$ 1Q�6Q   	 � Minimum$	 0Q�6]   $��0��6i   �� 5      -   5        
 ��Name$
 mCanvas��5      $��2��6&   $��0��63   $��2��6C   .+�5!      7  1System.Windows.Forms.Integration.WindowsFormsHost  ��6#   ��  �� ElementName$ mCanvas�� �� Path$ ActualWidth���5"      �� $ mCanvas��$ ActualHeight�   Child. 5#          System.Windows.Forms.Panel  -   5        mVideoPanel��"   System.Windows.Forms.Control   	BackColorU QSystem.Drawing, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a#  System.Drawing.ColorConverter$
 Blue 5#   4      Height$	 100��6E      Width$	 100��6R   5$      � 5'      -   5         �Name$ mWebCamParametrsPanel��5'       �� Bottom$
 -150��66   ��6K   �� $ mCanvas��$ ActualWidth� 5(      �� 5)      $
��Auto �6(   �� 5*      .+�5,      �� -   5        $ mShowBtn��5,   4   $	��100��6F   $��40��6R   $��22(�6^   $
��Show��6l   $��0��6{   $��0��6�   ��5-      �� 5.       �� Triggers 5/   "   �� 50   &    �� Binding 62   �� $ mMediaTypesComboBox��$ SelectedIndex� �� Value -151   .   .a�52   *   ��  �� Property � {� �� Value .  53   -   54   '   56      58      �� 59      -	   5         ��Name$ mWebCamParametrsTab��59   A   
�������$	��150��64   $��1��6^   $��0��6k   ��5:      �� 5;       �� Source# [ 6&    �� XPath$ Parametrs/Group��5<      ��5>      �� 5?      .R�5@   "   ��  ��Name$ mTitle��6:   $��18(�6,   ��6J   �� $ @Title��5A       �� ContentTemplate 5C      �� 5D      .R�5E   "   �� ��5F   &   �� 5G   *   $	 ./*��62   ��5I   &   �� 5J   *   .R�5K   .   �  � 	ItemWidth$	 170��68    � Orientation$ Vertical=�6H   5L   +   ��5N   &   �� 5O   *   .R�5P   .   �  5Q   2   �� 5R   6   $
��Auto �6D   �� 5S   6   .+�5U   2   �� $ mTitle��6g   $��0��6<   $��0��6L   $��18(�6Y   ��6w   �� $ @Title��� 5V   2   $��0��67   $��1��6G    5W   6   �� 5X   :   �� 5Y   :   $
��Auto �6K   .+�5[   6   ��  ��Name$ mParametrSlider��6z     �� IsSnapToTickEnabled  . $��0��5d   =   $��0��6M   {�5[   =   �� $ 
mFlagChBox��$ 	IsChecked�	 6�   �� $
 @Min�� 5\   =   �� $
 @Max�� 5]   =   �� $ @CurrentValue��
! � Tag! 5^   =   �� " � ToolTip" 5_   =   �� # �� Self$ �� RelativeSource#$ Z# $ Value�% � SmallChange% 5`   =   �� $ @Step��& � LargeChange& 5a   =   �� $ @Step��' �� TickFrequency' 5c   =   �� $ @Step��5d   M   �� 5f   6   ( ��Name$( 
mFlagChBox��6?   $��1��6p   $��0��6�   ! 6S   �� $ @Index��6�   5g   3   5h   /   ) �� Triggers) 5i   .   �� 5j   2   $ 2��6^    6>   �� $ @Flag��.a�5k   6   �� * �� 
TargetName$* 
mFlagChBox��6=    �@d�	IsChecked$
 TrueY�6j   5l   3   �� 5m   2   $ 1��6^    6>   �� $ @Flag��.a�5n   6   �� $* 
mFlagChBox��6=    �@d�	IsChecked$ FalseY�6j   5o   3   5q   +   5s   #   5t      5v      5w      5z      5{      5|      5}      