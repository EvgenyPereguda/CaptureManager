   M S B A M L   `   `   `  ����  WPFAreaScreenRecorder&     WPFAreaScreenRecorder.AreaWindow   5   	   R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5   	   R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5   	     � AllowsTransparency  .  � Topmost .  � WindowStyle$
 None=�5   	   
������  � 
ResizeMode$ NoResize=�5   "    � 	Resources 5         MarchingAnts g� 	BeginTime �� 
TargetName �� TargetProperty g� RepeatBehavior	 O� KeyTime
 O� Value��%  &	        �� 5	   
   .��5
      U� $ 00:00:00X�6,   $ border��5   ,   $ (Shape.StrokeDashOffset)�5   ,   $ Forever��5   ,   .\�5      �� $	 00:00:00��6'   $
 0Q�6:   �� 5      $	 00:00:00.5000000��6'   $
 10Q�6B   5      5      5       � Triggers 5      :� 5   
    :� RoutedEvent$ FrameworkElement.Loaded��6   .[�5      �� #��[  6   5      .��5      � -   5         �Name$ 
LayoutRoot��5      ���	#6f000000.+�5   
   (�  (� Stretch$ Uniform=�6   .��5      � .+�5      �� $��Selection Area��6   ���	#afffffff$��5,0,5,2q�6I   5      5      �� 5   
   -   5         ��Name$ canvas��5      ���	#00000000.+�5       � -   5        $ 	rectangle��5    >   ���	#14000000$y�Hidden=�6*   .+�5!      (� $ Uniform=�6   .��5"      �� $��10q�6     �� TextAlignment$ Center=�6,   $
��Bold"�6C   
�������$��Center=�6h   $��Center=�6�   '%Press Left Button of Mouse for moving�� 5$      �� 6&   +)For selection click Right Button of Mouse5&      5'      �� 5(      -   5         ��Name$ border��5)      ���	#00000000 �� StrokeDashArray$ 5R�5(   -   ���	#FFFFFFFF �� StrokeDashOffset$ 0Q�5*      $��1��6-    �� RadiusX$ 0��6A    �� RadiusY$ 0��6M    � ContextMenu 52      �� 53      .@�54      m� -   5        $��Select region��54   >   55      57      58       5:      �� 5;      .@�5<      m� -   5        $��Select whole area��5<   8   5=      5?      5@      5A      