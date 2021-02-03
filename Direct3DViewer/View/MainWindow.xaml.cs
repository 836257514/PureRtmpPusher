using Direct3DViewer.ViewModel;
using System.Windows;

namespace Direct3DViewer.View
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        D3DImageSource _imageSource = new D3DImageSource();

        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnWindowLoaded;
            DataContext = new MainWindowViewModel(_imageSource);
        }

        private void OnWindowLoaded(object sender, RoutedEventArgs e)
        {
            imgHoster.Source = _imageSource.ImageSource;
        }
    }
}
