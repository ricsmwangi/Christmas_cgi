# 🎄 Christmas CGI Web App - Deployment Guide

## 🌐 Deployment Options

### Option 1: Free CGI Hosting (Recommended)
**Best for sharing with friends!**

#### 000webhost (Free)
1. Sign up at https://www.000webhost.com/
2. Upload `main.cgi` to `public_html/cgi-bin/`
3. Set permissions: `chmod 755 main.cgi`
4. Access at: `https://yourdomain.000webhostapp.com/cgi-bin/main.cgi`

#### InfinityFree
1. Sign up at https://www.infinityfree.net/
2. Upload `main.cgi` to `htdocs/cgi-bin/`
3. Enable CGI in control panel
4. Access at: `https://yourdomain.epizy.com/cgi-bin/main.cgi`

### Option 2: VPS/Cloud Hosting
**For more control**

#### DigitalOcean Droplet ($6/month)
```bash
# Install Apache with CGI
sudo apt update
sudo apt install apache2
sudo a2enmod cgi

# Upload files to /usr/lib/cgi-bin/
sudo cp main.cgi /usr/lib/cgi-bin/
sudo chmod +x /usr/lib/cgi-bin/main.cgi

# Restart Apache
sudo systemctl restart apache2
```

#### Heroku (Free tier available)
```bash
# Create Heroku app
heroku create your-christmas-app

# Deploy
git push heroku main
```

### Option 3: Local Network Sharing
**Share on your local network**

```bash
# Get your local IP
hostname -I

# Start server accessible on network
cd /path/to/cgi-bin
python3 -m http.server --cgi --bind 0.0.0.0 8080

# Others can access at: http://YOUR_IP:8080/cgi-bin/main.cgi
```

## 📁 Files to Upload

Upload this file to your web server's `cgi-bin` directory:
- `main.cgi` - Santa's Christmas Mini Market (unified application)

## ⚙️ Server Requirements

- **CGI Support**: Apache/Nginx with CGI enabled
- **Permissions**: CGI files must be executable (755)
- **Architecture**: Linux/x86_64 (compiled for your server)

## 🚀 Quick Deploy Script

```bash
#!/bin/bash
# Deploy to web server via SCP
scp main.cgi user@yourserver.com:/var/www/cgi-bin/
ssh user@yourserver.com "chmod +x /var/www/cgi-bin/main.cgi"
```

## 🎯 URLs After Deployment

- **Main Marketplace**: `https://yourdomain.com/cgi-bin/main.cgi`
- **Tree Generator**: `https://yourdomain.com/cgi-bin/main.cgi?action=tree`
- **Card Creator**: `https://yourdomain.com/cgi-bin/main.cgi?action=card`
- **Secret Santa**: `https://yourdomain.com/cgi-bin/main.cgi?action=santa`
- **Countdown**: `https://yourdomain.com/cgi-bin/main.cgi?action=countdown`
- **About**: `https://yourdomain.com/cgi-bin/main.cgi?action=about`

## 🎄 Merry Christmas & Happy Deploying!

Your pure C Christmas web app is ready to spread holiday cheer! 🎅