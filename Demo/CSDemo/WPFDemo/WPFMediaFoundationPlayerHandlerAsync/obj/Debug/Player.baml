   M S B A M L   `   `   `  ����(  $WPFMediaFoundationPlayerHandlerAsync1    +WPFMediaFoundationPlayerHandlerAsync.Player   5      R NWindowsBase, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35\ XPresentationFramework, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35W SPresentationCore, Version=4.0.0.0, Culture=neutral, PublicKeyToken=31bf3856ad364e35D 9http://schemas.microsoft.com/winfx/2006/xaml/presentation    5      R NSystem.Xaml, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:x,http://schemas.microsoft.com/winfx/2006/xaml     5      Jmc;http://schemas.openxmlformats.org/markup-compatibility/2006     5      @d2http://schemas.microsoft.com/expression/blend/2008     5      -   5        
������ 
���  ��$��1q�5       }�. .��5      �   � ColumnDefinitions  5   
   �� 5      $
��Auto �6   �� 5      $
��Auto �6   �� 5      �� 5      $
��Auto �6   .+�5   
   �� -   5         ��Name$ m_SelectVideoFileBtn��5   �   $��Bottom=�6   $��Center=�6,   
����ح�$��30��6�   .��5      �� $��Images/film.png��6   5      �� 5   
   -   5        $ mPlayPauseBtn��5   <   $��10, 0q�6-   {�.  $��Bottom=�6e   $��Center=�6�   
����ح�$��1��6�   $��30��6�   .��5      �� -   5         ��Name$ 	mImageBtn��5      $��Images/play-button.png��6'   5      �� 5    
   -   5         ��Name$ mSlider��5    �    � Minimum$ 0Q�6�    � Maximum$	 100Q�6�   $��Bottom=�6�   $��25��6�   $��2��6  n� 5!   
   -   5         n�Name$ myThumb��5"   6   $��Bottom=�5!      $��3��6+   
����  �$��20��5"      $��20��6   ��5%      �� 5&       �� 
TargetType" n�6    �� Triggers 5'      P� 5(      	 P� Property	 �@J�IsMouseOver
 P� Value
 . . �5)      ��  �� Property � �� �� Value
 � ���5*   1   5+      5-      ��5/      �� 50      .R�51      �� .+�52      F� 
������$��1��54   #   "����        р�    �    ��	��53   #   �� *
Background+ �� RelativeSource ��  �� Mode$ FindAncestor=� �� AncestorType$ ThumbL�56      F� 57      
������$��1��59   #   "���� -1 -1� -1    �     -1	��58   #   �� *
Background+ �� $ FindAncestor=�$ ThumbL�5;      5<      5=      5?      5@      5A      