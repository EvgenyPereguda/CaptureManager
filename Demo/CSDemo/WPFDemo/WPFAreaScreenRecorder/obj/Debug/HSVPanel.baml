   M S B A M L   `   `   `  ����  WPFAreaScreenRecorder$    WPFAreaScreenRecorder.HSVPanel   5      R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      -   5        ��5	      �� 5
   
     �� 
TargetType"  ��6   .�5      ��  �� Property � �� �� Value$ 18(�6)   5      .��5      �  � RowDefinitions 5   
   �� 5      $
��Auto �6   �� 5      �� 5      $
��Auto �6   �� 5      $
��Auto �6   .+�5   
   �� $��Hue-Saturation��6    �� TextAlignment$ Center=�6*   $��0��6y   $��0��6�   ��6A   ��  �� ElementName$ hsImage�� �� Path$ ActualWidth�6�   �� 5   
   -   5         ��Name$ hsImage��5   H   $��Center=�6   $��Center=�6-   $��Uniform=�6Y   $��1��6k   $��0��6x   $ ��Assets\Images\HueSalur.png��6�   �� 5   
   -   5         ��Name$ 
backCanvas��5   �  
������ $��Center=�6�   $��Center=�6�   $��1��6�  $��0��6�  ��6  �� $ hsImage��$ ActualHeight���6M  �� $ hsImage��$ ActualWidth�5      �� 5   
   $��
Brightness��6   $ Center=�6&   $��2��6u   $��0��6�   ��6=   �� $ hsImage��$ ActualWidth�6�   �� 5   
   -   5        	 ��Name$	 valueSlider��5   9   
 � Minimum$
 0Q�6N    � Maximum$	 100Q�6Z    � Value$ 50Q�6h   $
��0,10q�6�   $��3��6�   $��0��6�   ��6s   �� $ hsImage��$ ActualWidth�6�   5      5      