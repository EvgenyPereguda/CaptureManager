﻿#pragma checksum "..\..\AreaWindow.xaml" "{8829d00f-11b8-4213-878b-770e8597ac16}" "A5F29998993A2A027AB57ED05A9BB875E331AE2A6E35BF85D38D0D5E9D0554A8"
//------------------------------------------------------------------------------
// <auto-generated>
//     Этот код создан программой.
//     Исполняемая версия:4.0.30319.42000
//
//     Изменения в этом файле могут привести к неправильной работе и будут потеряны в случае
//     повторной генерации кода.
// </auto-generated>
//------------------------------------------------------------------------------

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Automation;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Ink;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Media.Effects;
using System.Windows.Media.Imaging;
using System.Windows.Media.Media3D;
using System.Windows.Media.TextFormatting;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Shell;


namespace WPFAreaScreenRecorder {
    
    
    /// <summary>
    /// AreaWindow
    /// </summary>
    public partial class AreaWindow : System.Windows.Window, System.Windows.Markup.IComponentConnector {
        
        
        #line 25 "..\..\AreaWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Grid LayoutRoot;
        
        #line default
        #line hidden
        
        
        #line 31 "..\..\AreaWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Canvas canvas;
        
        #line default
        #line hidden
        
        
        #line 32 "..\..\AreaWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Controls.Grid rectangle;
        
        #line default
        #line hidden
        
        
        #line 41 "..\..\AreaWindow.xaml"
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1823:AvoidUnusedPrivateFields")]
        internal System.Windows.Shapes.Rectangle border;
        
        #line default
        #line hidden
        
        private bool _contentLoaded;
        
        /// <summary>
        /// InitializeComponent
        /// </summary>
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        public void InitializeComponent() {
            if (_contentLoaded) {
                return;
            }
            _contentLoaded = true;
            System.Uri resourceLocater = new System.Uri("/WPFAreaScreenRecorder;component/areawindow.xaml", System.UriKind.Relative);
            
            #line 1 "..\..\AreaWindow.xaml"
            System.Windows.Application.LoadComponent(this, resourceLocater);
            
            #line default
            #line hidden
        }
        
        [System.Diagnostics.DebuggerNonUserCodeAttribute()]
        [System.CodeDom.Compiler.GeneratedCodeAttribute("PresentationBuildTasks", "4.0.0.0")]
        [System.ComponentModel.EditorBrowsableAttribute(System.ComponentModel.EditorBrowsableState.Never)]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Design", "CA1033:InterfaceMethodsShouldBeCallableByChildTypes")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Maintainability", "CA1502:AvoidExcessiveComplexity")]
        [System.Diagnostics.CodeAnalysis.SuppressMessageAttribute("Microsoft.Performance", "CA1800:DoNotCastUnnecessarily")]
        void System.Windows.Markup.IComponentConnector.Connect(int connectionId, object target) {
            switch (connectionId)
            {
            case 1:
            this.LayoutRoot = ((System.Windows.Controls.Grid)(target));
            return;
            case 2:
            this.canvas = ((System.Windows.Controls.Canvas)(target));
            return;
            case 3:
            this.rectangle = ((System.Windows.Controls.Grid)(target));
            return;
            case 4:
            this.border = ((System.Windows.Shapes.Rectangle)(target));
            
            #line 43 "..\..\AreaWindow.xaml"
            this.border.MouseLeftButtonDown += new System.Windows.Input.MouseButtonEventHandler(this.rectangle_MouseLeftButtonDown);
            
            #line default
            #line hidden
            
            #line 44 "..\..\AreaWindow.xaml"
            this.border.MouseLeftButtonUp += new System.Windows.Input.MouseButtonEventHandler(this.rectangle_MouseLeftButtonUp);
            
            #line default
            #line hidden
            
            #line 45 "..\..\AreaWindow.xaml"
            this.border.MouseMove += new System.Windows.Input.MouseEventHandler(this.rectangle_MouseMove);
            
            #line default
            #line hidden
            
            #line 46 "..\..\AreaWindow.xaml"
            this.border.MouseLeave += new System.Windows.Input.MouseEventHandler(this.rectangle_MouseLeave);
            
            #line default
            #line hidden
            return;
            case 5:
            
            #line 52 "..\..\AreaWindow.xaml"
            ((System.Windows.Controls.MenuItem)(target)).Click += new System.Windows.RoutedEventHandler(this.MenuItem_Click);
            
            #line default
            #line hidden
            return;
            case 6:
            
            #line 60 "..\..\AreaWindow.xaml"
            ((System.Windows.Controls.MenuItem)(target)).Click += new System.Windows.RoutedEventHandler(this.MenuItem_Click_1);
            
            #line default
            #line hidden
            return;
            }
            this._contentLoaded = true;
        }
    }
}

